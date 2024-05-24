#include "MatrixMultiplyApplication.h"

#include <FastCG/Platform/Timer.h>

#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <memory>
#include <random>
#include <vector>

#define PRINT_RESULTS 0
#define FRAME_BASED 1
#define ASSERT 0

namespace
{
    Matrix MakeZeroMatrix(size_t length)
    {
        return std::make_unique<float[]>(length);
    }

    Matrix MakeRandomMatrix(size_t length)
    {
        static std::mt19937 s_prng{std::random_device{}()};
        static std::uniform_real_distribution<float> s_dist(0, 1);
        auto matrix = MakeZeroMatrix(length);
        for (size_t i = 0; i < length; ++i)
        {
            matrix[i] = s_dist(s_prng);
        }
        return matrix;
    }
}

bool MatrixMultiplyApplication::ParseCommandLineArguments(int argc, char **argv)
{
    struct Arg
    {
        const char *name;
        int *pValue;
    };
    for (int i = 1; i < argc; i += 2)
    {
        for (const auto &rArg : {Arg{"--count", &mCount}, Arg{"--M", &mM}, Arg{"--N", &mN}, Arg{"--K", &mK}})
        {
            if (std::strcmp(argv[i], rArg.name) == 0)
            {
                if (i >= argc)
                {
                    std::cout << "No value for argument: " << argv[i] << std::endl;
                    return false;
                }
                *rArg.pValue = std::atoi(argv[i + 1]);
                goto __argFound;
            }
        }
        std::cout << "Unknown argument: " << argv[i] << std::endl;
        return false;
    __argFound:
        continue;
    }
    return true;
}

void MatrixMultiplyApplication::OnPrintUsage()
{
    std::cout << "Usage: " << std::endl
              << "\t--count <int>" << std::endl
              << "\t--M <int>" << std::endl
              << "\t--N <int>" << std::endl
              << "\t--K <int>" << std::endl
              << std::endl;
}

void MatrixMultiplyApplication::OnStart()
{
#if !FRAME_BASED
    RecordCommands();
    GraphicsSystem::GetInstance()->Submit();
    ProcessResults();
    Exit();
#endif
}

static size_t sFramesElapsed = 0;

void MatrixMultiplyApplication::OnFrameStart(double deltaTime)
{
#if FRAME_BASED
    if (sFramesElapsed == 1)
    {
        RecordCommands();
    }
#endif
}

void MatrixMultiplyApplication::OnFrameEnd()
{
#if FRAME_BASED
    if (sFramesElapsed == 1)
    {
        GraphicsSystem::GetInstance()->WaitLastFrame();
        ProcessResults();
        Exit();
    }
    sFramesElapsed++;
#endif
}

void MatrixMultiplyApplication::RecordCommands()
{
    auto *pShader = GraphicsSystem::GetInstance()->FindShader("ScalarLoop");

    mpGraphicsContext = GraphicsSystem::GetInstance()->CreateGraphicsContext({"Application Context"});
    mpGraphicsContext->Begin();

    std::vector<Matrix> A_values(mCount), B_values(mCount), C_outputs(mCount);
    std::vector<const Buffer *> A_buffers(mCount), B_buffers(mCount);
    mC_buffers.resize(mCount);
    mC_values_cpu.resize(mCount);

    const auto A_length = mM * mK;
    const auto B_length = mK * mN;
    const auto C_length = mM * mN;

    for (int i = 0; i < mCount; ++i)
    {
        A_values[i] = MakeRandomMatrix(A_length);
        B_values[i] = MakeRandomMatrix(B_length);

        A_buffers[i] = GraphicsSystem::GetInstance()->CreateBuffer({"A_" + std::to_string(i),
                                                                    BufferUsageFlagBit::SHADER_STORAGE,
                                                                    A_length * sizeof(float),
                                                                    &A_values[i][0]
#if FASTCG_VULKAN
                                                                    ,
                                                                    {},
                                                                    true,
                                                                    mpGraphicsContext
#endif
        });

        B_buffers[i] = GraphicsSystem::GetInstance()->CreateBuffer({"B_" + std::to_string(i),
                                                                    BufferUsageFlagBit::SHADER_STORAGE,
                                                                    B_length * sizeof(float),
                                                                    &B_values[i][0]
#if FASTCG_VULKAN
                                                                    ,
                                                                    {},
                                                                    true,
                                                                    mpGraphicsContext
#endif
        });

        mC_buffers[i] = GraphicsSystem::GetInstance()->CreateBuffer(
            {"C_" + std::to_string(i),
             BufferUsageFlagBit::SHADER_STORAGE | BufferUsageFlagBit::DYNAMIC,
             C_length * sizeof(float),
             nullptr
#if FASTCG_VULKAN
             ,
             {},
             true,
             mpGraphicsContext
#endif
            });

        mC_values_cpu[i] = MakeZeroMatrix(C_length);
    }

    auto start = Timer::GetTime();
    for (int i = 0; i < mCount; ++i)
    {
        const auto &rA = A_values[i];
        const auto &rB = B_values[i];
        auto &rC = mC_values_cpu[i];
        for (int m = 0; m < mM; ++m)
        {
            for (int n = 0; n < mN; ++n)
            {
                float c = 0;
                for (int k = 0; k < mK; ++k)
                {
                    c += rA[m * mK + k] * rB[k * mN + n];
                }
                rC[m * mN + n] = c;
            }
        }
    }
    auto end = Timer::GetTime();
    mCpuElapsedTime = end - start;

    struct
    {
        unsigned int mM, mN, mK;
    } sizes{(unsigned int)mM, (unsigned int)mN, (unsigned int)mK};
    const Buffer *pSizesBuffer = GraphicsSystem::GetInstance()->CreateBuffer({"Sizes",
                                                                              BufferUsageFlagBit::UNIFORM,
                                                                              sizeof(sizes),
                                                                              &sizes
#if FASTCG_VULKAN
                                                                              ,
                                                                              {},
                                                                              true,
                                                                              mpGraphicsContext
#endif
    });

    mpGraphicsContext->BindShader(pShader);
    mpGraphicsContext->BindResource(pSizesBuffer, "Sizes");
    mpGraphicsContext->PushDebugMarker("MatrixMultiplication");
    for (int i = 0; i < mCount; ++i)
    {
        mpGraphicsContext->PushDebugMarker(
            ("C_" + std::to_string(i) + " = A_" + std::to_string(i) + " * B_" + std::to_string(i)).c_str());
        {
            mpGraphicsContext->BindResource(A_buffers[i], "A");
            mpGraphicsContext->BindResource(B_buffers[i], "B");
            mpGraphicsContext->BindResource(mC_buffers[i], "C");
            mpGraphicsContext->Dispatch(mN, mM, 1);
        }
        mpGraphicsContext->PopDebugMarker();
    }
    mpGraphicsContext->PopDebugMarker();
    mpGraphicsContext->AddMemoryBarrier();
    mpGraphicsContext->End();
}

void MatrixMultiplyApplication::ProcessResults()
{
    const auto PrintCs = [&](const std::vector<Matrix> &rCs) {
#if PRINT_RESULTS
        for (int i = 0; i < mCount; ++i)
        {
            std::cout << "\t\t[" << i << "]" << std::endl;
            for (size_t m = 0, c = 0; m < mM; ++m)
            {
                std::cout << "\t\t\t";
                for (size_t n = 0; n < mN; ++n, ++c)
                {
                    std::cout << rCs[i][c] << ' ';
                }
                std::cout << std::endl;
            }
        }
#endif
    };

    const auto C_length = mM * mN;

    std::cout << std::fixed;

    std::cout << "[CPU] " << mCpuElapsedTime * 1000.0 << "ms" << std::endl;
    PrintCs(mC_values_cpu);

    std::vector<Matrix> C_values_gpu(mCount);
    for (int i = 0; i < mCount; ++i)
    {
        C_values_gpu[i] = MakeZeroMatrix(C_length);
        mpGraphicsContext->Copy(&C_values_gpu[i][0], mC_buffers[i], 0, C_length * sizeof(float));
    }

    std::cout << "[GPU] " << mpGraphicsContext->GetElapsedTime() * 1000.0 << "ms" << std::endl;
    PrintCs(C_values_gpu);

#if ASSERT
    auto AreEqual = [](float a, float b, float epsilon = 1e-3) { return std::fabs(a - b) < epsilon; };

    bool diff = false;
    for (size_t i = 0; i < mCount; ++i)
    {
        for (size_t m = 0, c = 0; m < mM; ++m)
        {
            for (size_t n = 0; n < mN; ++n, ++c)
            {
                if (!AreEqual(mC_values_cpu[i][c], C_values_gpu[i][c]))
                {
                    std::cout << " C_" << i << "[" << m << ", " << n
                              << "] GPU/GPU values differ too much (CPU: " << mC_values_cpu[i][c]
                              << " / GPU: " << C_values_gpu[i][c] << ")" << std::endl;
                    diff = true;
                }
            }
        }
    }
    assert(!diff);
#endif
}

FASTCG_MAIN(MatrixMultiplyApplication)