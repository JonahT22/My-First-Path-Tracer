#include <iostream>
#include <string>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "Image.h"

using namespace std;
using namespace glm;

};

int main(int argc, char **argv)
{
	if(argc < 4) {
		cout << "Usage: ./A6 <SCENENUMBER> <IMAGE SIZE> <IMAGE FILENAME>" << endl;
		return 0;
	}
	int sceneNum = atoi(argv[1]);
	int imageSize = atoi(argv[2]);
	string fileName(argv[3]);
	shared_ptr<Image> outputImage = make_shared<Image>(imageSize, imageSize);

		}
	}
	
	return 0;
}
