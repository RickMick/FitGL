#include "Material.h"

using namespace fgl;

void PhongMaterial::bind(ge::gl::ProgramS & program){
	if (program->getUniformLocation("diffuse") != -1)
		program->set4fv("diffuse", glm::value_ptr(diffuse));
	if (diffuseTex->getId() != 0)
		diffuseTex->bind(0);
}
