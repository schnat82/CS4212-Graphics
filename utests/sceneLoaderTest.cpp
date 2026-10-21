#include <iostream>

#include "SceneContainer.h"
#include "SceneLoader.h"
#include "SceneParser_JSON.h"

int main(int argc, char *argv[]) {
  SceneContainer scene;

  std::shared_ptr<ISceneLoader> loader =
      std::static_pointer_cast<ISceneLoader>(std::make_shared<SceneLoader>(scene));
  SceneParser_JSON parser(loader);

  std::string filename = argv[1];
  parser.parseFileData(filename);
}
