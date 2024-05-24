#ifndef MATRIX_MULTIPLY_APPLICATION_H
#define MATRIX_MULTIPLY_APPLICATION_H

#include <FastCG/Platform/Application.h>
#include <FastCG/Graphics/GraphicsSystem.h>

#include <vector>

using namespace FastCG;

using Matrix = std::unique_ptr<float[]>;

class MatrixMultiplyApplication : public Application
{
protected:
	bool ParseCommandLineArguments(int argc, char **argv);
	void OnPrintUsage() override;
	void OnStart() override;
	void OnFrameStart(double deltaTime) override;
	void OnFrameEnd() override;

private:
	int mCount{16};
	int mM{128};
	int mN{128};
	int mK{128};
	GraphicsContext *mpGraphicsContext;
	std::vector<const Buffer *> mC_buffers;
	std::vector<Matrix> mC_values_cpu;
	double mCpuElapsedTime{0};

	void RecordCommands();
	void ProcessResults();
};

#endif
