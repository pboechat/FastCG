#ifndef MODELIMPORTER_H_
#define MODELIMPORTER_H_

#include <Model.h>

#include <assimp/cimport.h>

#include <string>
#include <map>

class ModelImporter
{
public:
	static void Initialize();
	static Model* Import(const std::string& rFileName);
	static void Dispose();

private:
	static std::map<std::string, Model*> s_mModelsByName;
	static aiLogStream s_mConsoleLogStream;
	static aiLogStream s_mFileLogStream;

};

#endif