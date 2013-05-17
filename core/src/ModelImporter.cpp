#include <ModelImporter.h>

std::map<std::string, Model*> ModelImporter::s_mModelsByName;
aiLogStream ModelImporter::s_mConsoleLogStream;
aiLogStream ModelImporter::s_mFileLogStream;

void ModelImporter::Initialize()
{
	s_mConsoleLogStream = aiGetPredefinedLogStream(aiDefaultLogStream_STDOUT, 0);
	aiAttachLogStream(&s_mConsoleLogStream);
	s_mFileLogStream = aiGetPredefinedLogStream(aiDefaultLogStream_FILE, "assimpLog.txt");
	aiAttachLogStream(&s_mFileLogStream);
}

void ModelImporter::Dispose()
{
	std::map<std::string, Model*>::iterator it = s_mModelsByName.begin();
	while (it != s_mModelsByName.end())
	{
		delete it->second;
		it++;
	}
	s_mModelsByName.clear();

	aiDetachAllLogStreams();
}

Model* ModelImporter::Import(const std::string& rFileName)
{
	std::map<std::string, Model*>::iterator it = s_mModelsByName.find(rFileName);

	if (it != s_mModelsByName.end())
	{
		return it->second;
	}

	Model* pModel = new Model(rFileName);
	s_mModelsByName.insert(std::make_pair(rFileName, pModel));
	return pModel;
}