#ifndef MODELIMPORTAPPLICATION_H_
#define MODELIMPORTAPPLICATION_H_

#include <Application.h>
#include <ModelImporter.h>

class ModelImportApplication : public Application
{
public:
	ModelImportApplication();

protected:
	virtual void OnStart();

private:
	GameObject* ImportModelToScene(const std::string& rFileName, unsigned int importSettings = ModelImporter::IS_QUALITY_GOOD);

};

#endif