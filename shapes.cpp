#include "../../Header/OpenglStuff/shapes.h"
#include "../../Header/OpenglStuff/shaders.h"
#include "../../Header/OpenglStuff/light.h"
#include "../../Header/globarVar.h"

#include <string>

void shape2D_properties::uniLocSetValues() {
	program2d.use();

	glUniform2fv(uniLoc2D.offsetPos, 1, glm::value_ptr(offsetPos));
	glUniformMatrix4fv(uniLoc2D.ortho, 1, GL_FALSE, glm::value_ptr(ortho2D));
	glUniform4fv(uniLoc2D.color, 1, glm::value_ptr(color));
	glUniform2fv(uniLoc2D.size, 1, glm::value_ptr(size));
	glUniform2fv(uniLoc2D.positionNDC, 1, glm::value_ptr(positionNDC));

	glUniform1i(uniLoc2D.zIndex, zIndex);
}

void shape_properties::uniLocSetValues(unsigned int prog, programType pType, shadowFBO_holder* sFBO) {
	uni3Dlocation* uniloc;
	//setUniLocID(prog, pType);

	glUseProgram(prog);
	

	switch (pType) {
	case programType::threeDim:
		uniloc = &uniLoc3D;
		break;
	case programType::reflective3Dim:
		uniloc = &uniLoc3Dreflective;
		break;
	case programType::instancing3Dim:
		uniloc = &uniLocInstancing3Dim;
		break;
	case programType::reflectiveInstancing3Dim:
		uniloc = &uniLocReflectiveInstancing3Dim;
		break;

	case programType::threeDimNormal:
		uniloc = &uniLocNormal3D;
		break;
	case programType::reflective3DimNormal:
		uniloc = &uniLocNormal3Dreflective;
		break;
	case programType::instancing3DimNormal:
		uniloc = &uniLocNormalInstancing3Dim;
		break;
	case programType::reflectiveInstancing3DimNormal:
		uniloc = &uniLocNormalReflectiveInstancing3Dim;
		break;
	/*case programType::shadowMap:
		glUniformMatrix4fv(shadowUniLoc.model, 1, GL_FALSE, glm::value_ptr(modelMat));
		glUniformMatrix4fv(
			shadowUniLoc.lightSpaceMatrix, 1, GL_FALSE, glm::value_ptr(sFBO->getViewShadow_P())
		);
		return;
	case programType::shadowInstanceMap:
		glUniformMatrix4fv(shadowInstancingUniLoc.model, 1, GL_FALSE, glm::value_ptr(modelMat));
		glUniformMatrix4fv(
			shadowInstancingUniLoc.lightSpaceMatrix, 1, GL_FALSE, glm::value_ptr(sFBO->getViewShadow_P())
		);
		return;*/
	case programType::shadowDepthMap: {
		shadowFBO* shadow = static_cast<shadowFBO*>(sFBO);
		glUniformMatrix4fv(shadowDepthUniLoc.model, 1, GL_FALSE, glm::value_ptr(modelMat));
		glUniformMatrix4fv(
			shadowDepthUniLoc.lightSpaceMatrix, 1, GL_FALSE, glm::value_ptr(shadow->getViewShadow_P())
		);
		return;
	}
	case programType::shadowVisualMap: {
		shadowFBO* shadow = static_cast<shadowFBO*>(sFBO);
		glUniformMatrix4fv(shadowVisualUniLoc.model, 1, GL_FALSE, glm::value_ptr(modelMat));
		glUniformMatrix4fv(
			shadowVisualUniLoc.lightSpaceMatrix, 1, GL_FALSE, glm::value_ptr(shadow->getViewShadow_P())
		);
		return;
	}
	case programType::shadowDepthInstanceMap: {
		/*std::cout << "--- shadowDepthInstanceMap Matrix (modelMat) ---" << std::endl;
		for (int row = 0; row < 4; ++row) {
			for (int col = 0; col < 4; ++col) {
				std::cout << modelMat[col][row] << "\t";
			}
			std::cout << std::endl;
		}
		*/
		//std::cout << "---------------------------------------------" << std::endl;
		shadowFBO* shadow = static_cast<shadowFBO*>(sFBO);
		glUniformMatrix4fv(shadowDepthInstancingUniLoc.model, 1, GL_FALSE, glm::value_ptr(modelMat));
		glUniformMatrix4fv(
			shadowDepthInstancingUniLoc.lightSpaceMatrix, 1, GL_FALSE, glm::value_ptr(shadow->getViewShadow_P())
		);
		return;
	}
	case programType::shadowDepthCubeMap:{
		//std::cout << "sdcm\n";
		cubeShadowFBO* shadow = static_cast<cubeShadowFBO*>(sFBO);
		glUniformMatrix4fv(shadowDepthCubeUniLoc.model, 1, GL_FALSE, glm::value_ptr(modelMat));
		glUniform1f(shadowDepthCubeUniLoc.far_plane, shadow->getProperty().far_plane);
		glUniform3fv(
			shadowDepthCubeUniLoc.lightpos, 1, 
			glm::value_ptr(shadow->getProperty().position)
		);

		std::array<glm::mat4, 6>& shadowView = shadow->getProperty().shadowView;
		for (int i = 0; i < 6; i++) {
			glUniformMatrix4fv(
				shadowDepthCubeUniLoc.shadowView[i], 1, 
				GL_FALSE, glm::value_ptr(shadowView[i])
			);
		}

		return;
	}
	case programType::shadowDepthInstanceCubeMap:{
		//std::cout << "sdicm\n";
		cubeShadowFBO* shadow = static_cast<cubeShadowFBO*>(sFBO);
		glUniformMatrix4fv(shadowDepthCubeInstancingUniLoc.model, 1, GL_FALSE, glm::value_ptr(modelMat));
		glUniform1f(shadowDepthCubeInstancingUniLoc.far_plane, shadow->getProperty().far_plane);
		glUniform3fv(
			shadowDepthCubeInstancingUniLoc.lightpos, 1, 
			glm::value_ptr(shadow->getProperty().position)
		);

		std::array<glm::mat4, 6>& shadowView = shadow->getProperty().shadowView;
		for (int i = 0; i < 6; i++) {
			glUniformMatrix4fv(
				shadowDepthCubeInstancingUniLoc.shadowView[i], 1, 
				GL_FALSE, glm::value_ptr(shadowView[i])
			);
		}

		return;
	}
	default:
		uniloc = &uniLoc3D;
		break;
	}

	// vertex uniforms
	// matrix

	glUseProgram(prog);

	//globalUni->setLightSpaceMatrix(); // placed somewhere in line 261 in main code
	int n = -1;
	glGetUniformiv(prog, uniloc->pointLight_shadowmap[0], &n);
	//std::cout << n << " ------------ pointLight_shadowmap[0]\n";
	if (highlightMode) {
		// later ignore this
		glUniform1i(uniloc->highlight, 1);

		glm::vec3 originalSize = size;
		setSize(size * thicknessHighlight);

		//std::cout << "testest=----------------------1\n";
		glUniformMatrix4fv(uniloc->model, 1, GL_FALSE, glm::value_ptr(modelMat));
		//glUniformMatrix4fv(uniloc->perspective, 1, GL_FALSE, glm::value_ptr(currentCamera->perspective));
		//glUniformMatrix4fv(uniloc->view, 1, GL_FALSE, glm::value_ptr(currentCamera->getView()));

		glUniform4fv(uniloc->highlightColor, 1, glm::value_ptr(highlightColor));

		setSize(originalSize);
		//std::cout << "im being called -----------------------------fgadadada" << std::endl;
		return;
	}

	glUniform1f(uniloc->reflective, getReflectiveness());
	glUniformMatrix4fv(uniloc->model, 1, GL_FALSE, glm::value_ptr(modelMat));

	// fragment uniforms
	glUniform1i(uniloc->highlight, 0);
	
	// cam pos,  color
	glm::vec3 curcamPos = currentCamera->getPosition();
	glm::vec3 curcamDir = currentCamera->frontVector;
	//std::cout << curcamDir.x << ", " << curcamDir.y << ", " << curcamDir.z << std::endl;
	glUniform3fv(uniloc->camPos, 1, glm::value_ptr(curcamPos));
	glUniform3fv(uniloc->camDir, 1, glm::value_ptr(curcamDir));

	glUniform4fv(uniloc->color, 1, glm::value_ptr(color));
	glUniform1f(uniloc->transparency, transparency);
	//std::cout << transparency << '\n';
	// materials

	int diffcount = getDiffuses().size();
	int speccount = getSpeculars().size();
	int dispcount = getDisplacements().size();
	int normcount = getNormals().size();

	glUniform1f(uniloc->material.shininess, shininess);

	//int offsetUnit = 0;
	int offsetTex = 0;
	for (int i = 0; i < diffcount; i++) {
		glActiveTexture(GL_TEXTURE0 + offsetTex + i);
		glBindTexture(GL_TEXTURE_2D, getDiffuses()[i].texbuf);
	}
	//offsetUnit += texDiffuseSize;
	offsetTex += texDiffuseSize;

	for (int i = 0; i < speccount; i++) {
		glActiveTexture(GL_TEXTURE0 + offsetTex + i);
		glBindTexture(GL_TEXTURE_2D, getSpeculars()[i].texbuf);
	}
	//offsetUnit += texSpecularSize;
	offsetTex += texSpecularSize;

	const std::vector<Texture>& dispVec = getDisplacements();
	for (int i = 0; i < dispcount; i++) {
		glActiveTexture(GL_TEXTURE0 + offsetTex + i);
		glBindTexture(GL_TEXTURE_2D, dispVec[i].texbuf);

		glUniform1f(uniloc->material.height_scale[i], dispVec[i].height_scale);
	}
	//offsetUnit += texDisplacementSize;
	offsetTex += texDisplacementSize;

	for (int i = 0; i < normcount; i++) {
		glActiveTexture(GL_TEXTURE0 + offsetTex + i);
		glBindTexture(GL_TEXTURE_2D, getNormals()[i].texbuf);
	}
	//offsetUnit += texNormalsSize;
	offsetTex += texNormalsSize;
	// 3 2 2 = 7 offset
	
	//glActiveTexture(GL_TEXTURE0);
	//////////

	glUniform1i(uniloc->material.diffuseSize, diffcount);
	glUniform1i(uniloc->material.specularSize, speccount);
	glUniform1i(uniloc->material.displacementSize, dispcount);
	glUniform1i(uniloc->material.normalsSize, normcount);
	
	// pointlight
	glUniform1i(uniloc->pointLightAmount, pointlights.size());

	for (int i = 0; i < pointlights.size(); i++) {
		decltype(uniloc->pointLight[i])& pointUniLoc = uniloc->pointLight[i];
		pointlight*& point = pointlights[i];

		glActiveTexture(GL_TEXTURE0 + offsetTex + i);
		glBindTexture(GL_TEXTURE_CUBE_MAP, point->shadow.getDepth_ID());

		glUniform1f(
			uniloc->far_plane_pointLight[i],
			pointlights[i]->shadow.getProperty().far_plane
		);

		//glActiveTexture(offsetTex + i);
		//glBindTexture(GL_TEXTURE_CUBE_MAP, point->shadow.getDepth_ID());
		//glUniform1i(uniloc->pointLight_shadowmap[i], offsetUnit + i);

		glUniform3fv(pointUniLoc.diffuse, 1, glm::value_ptr(point->diffuse));
		glUniform3fv(pointUniLoc.specular, 1, glm::value_ptr(point->specular));

		glUniform1f(pointUniLoc.constant, point->constant);
		glUniform1f(pointUniLoc.linear, point->linear);
		glUniform1f(pointUniLoc.quadratic, point->quadratic);

		glUniform3fv(pointUniLoc.color, 1, glm::value_ptr(point->color));

		glUniform3fv(pointUniLoc.position, 1, glm::value_ptr(point->getPosition()));
	}

	glUniform1i(uniloc->spotLightAmount, spotlights.size());
	
	//offsetUnit += MAX_POINT_LIGHT;
	offsetTex += MAX_POINT_LIGHT;
	for (int i = 0; i < spotlights.size(); i++) {
		decltype(uniloc->spotLight[i])& spotUniLoc = uniloc->spotLight[i];
		spotlight*& spot = spotlights[i];

		glActiveTexture(GL_TEXTURE0 + offsetTex + i);
		glBindTexture(GL_TEXTURE_2D, spot->shadow.getDepth_ID());

		glUniformMatrix4fv(
			uniloc->spotLight_lightMatrix[i], 1, 
			GL_FALSE, glm::value_ptr(spot->shadow.getViewShadow_P())
		);

		glUniform3fv(spotUniLoc.diffuse, 1, glm::value_ptr(spot->diffuse));
		glUniform3fv(spotUniLoc.specular, 1, glm::value_ptr(spot->specular));

		glUniform1f(spotUniLoc.constant, spot->constant);
		glUniform1f(spotUniLoc.linear, spot->linear);
		glUniform1f(spotUniLoc.quadratic, spot->quadratic);

		glUniform3fv(spotUniLoc.color, 1, glm::value_ptr(spot->color));

		glUniform3fv(spotUniLoc.position, 1, glm::value_ptr(spot->position));
		glUniform3fv(spotUniLoc.direction, 1, glm::value_ptr(spot->direction));

		glUniform1f(spotUniLoc.cutOff, spot->cutOff);
		glUniform1f(spotUniLoc.outerCutOff, spot->outerCutOff);
	}
	glActiveTexture(GL_TEXTURE0);

}

void shape2D_properties::setUniLocID(unsigned int program2d) {
	uniLoc2D.offsetPos = glGetUniformLocation(program2d, "offsetPos");
	uniLoc2D.ortho = glGetUniformLocation(program2d, "ortho");
	uniLoc2D.color = glGetUniformLocation(program2d, "color");
	uniLoc2D.positionNDC = glGetUniformLocation(program2d, "positionNDC");
	uniLoc2D.size = glGetUniformLocation(program2d, "size");
	uniLoc2D.zIndex = glGetUniformLocation(program2d, "zIndex");
}

void shape_properties::setUniLocID(unsigned int program, programType pType) {
	glUseProgram(program);
	// matrix

	uni3Dlocation* uniloc;

	switch (pType) { // IF CREATE NEW PROGRAM PLS MAKE A NEW GLOBAL UNILOC FOR IT AND PUT HERE
	case programType::threeDim:
		uniloc = &uniLoc3D;
		break;
	case programType::reflective3Dim:
		uniloc = &uniLoc3Dreflective;
		break;
	case programType::instancing3Dim:
		uniloc = &uniLocInstancing3Dim;
		break;
	case programType::reflectiveInstancing3Dim:
		uniloc = &uniLocReflectiveInstancing3Dim;
		break;

	case programType::threeDimNormal:
		uniloc = &uniLocNormal3D;
		break;
	case programType::reflective3DimNormal:
		uniloc = &uniLocNormal3Dreflective;
		break;
	case programType::instancing3DimNormal:
		uniloc = &uniLocNormalInstancing3Dim;
		break;
	case programType::reflectiveInstancing3DimNormal:
		uniloc = &uniLocNormalReflectiveInstancing3Dim;
		break;

	case programType::shadowMap:
	{
		unsigned int pSDT = programShadowDepthTest.getID();
		unsigned int pSVM = programShadowDepthVisual.getID();
		unsigned int pSIDT = programShadowInstanceDepthTest.getID();
		unsigned int pSCM = programShadowCubeMap.getID();
		unsigned int pISCM = programInstanceShadowCubeMap.getID();

		shadowDepthUniLoc.model = glGetUniformLocation(pSDT, "model");
		shadowDepthUniLoc.lightSpaceMatrix = glGetUniformLocation(pSDT, "lightSpaceMatrix");

		shadowVisualUniLoc.sampleDepth = glGetUniformLocation(pSVM, "sampleDepth");

		shadowDepthInstancingUniLoc.model = glGetUniformLocation(pSIDT, "model");
		shadowDepthInstancingUniLoc.lightSpaceMatrix = glGetUniformLocation(pSIDT, "lightSpaceMatrix");

		// cubemap (no visual just use renderdoc)
		shadowDepthCubeUniLoc.model = glGetUniformLocation(pSCM, "model");
		shadowDepthCubeUniLoc.lightpos = glGetUniformLocation(pSCM, "lightpos");
		shadowDepthCubeUniLoc.far_plane = glGetUniformLocation(pSCM, "far_plane");

		shadowDepthCubeInstancingUniLoc.model = glGetUniformLocation(pISCM, "model");
		shadowDepthCubeInstancingUniLoc.lightpos = glGetUniformLocation(pISCM, "lightpos");
		shadowDepthCubeInstancingUniLoc.far_plane = glGetUniformLocation(pISCM, "far_plane");

		for (int i = 0; i < 6; i++) {
			shadowDepthCubeUniLoc.shadowView[i] = glGetUniformLocation(
				pSCM, ("shadowViews[" + std::to_string(i) + "]").c_str()
			);

			shadowDepthCubeInstancingUniLoc.shadowView[i] = glGetUniformLocation(
				pISCM, ("shadowViews[" + std::to_string(i) + "]").c_str()
			);
		}

		return;
	}
	default:
		uniloc = &uniLoc3D;
		break;
	}
	
	uniloc->shadowMap = glGetUniformLocation(program, "shadowMap");

	uniloc->reflective = glGetUniformLocation(program, "reflective");
	uniloc->skybox = glGetUniformLocation(program, "skybox");

	uniloc->model = glGetUniformLocation(program, "model");

	/*std::cout << uniloc->model << "----------------------------------uniloc model | ";
	std::string str = 
		(pType == programType::threeDim) ? "threeDim" :
		(pType == programType::reflective3Dim) ? "reflective3Dim" :
		(pType == programType::instancing3Dim) ? "instancing3Dim" :
		(pType == programType::reflectiveInstancing3Dim) ? "reflectiveInstancing3Dim" :
		"unknown";
	std::cout << str << '\n';*/

	// no more, in global uniform (uniform buffer)
	//uniloc->view        = glGetUniformLocation(program3d, "view");
	//uniloc->perspective = glGetUniformLocation(program3d, "perspective");
	uniloc->highlight = glGetUniformLocation(program, "highlight");
	uniloc->highlightColor = glGetUniformLocation(program, "highlightColor");

	uniloc->color = glGetUniformLocation(program, "color");
	uniloc->camPos = glGetUniformLocation(program, "camPos");
	uniloc->camDir = glGetUniformLocation(program, "camDir");
	uniloc->transparency = glGetUniformLocation(program, "transparency");
	
	// material
	uniloc->material.diffuseSize = glGetUniformLocation(program, "material.diffuseSize");
	uniloc->material.specularSize = glGetUniformLocation(program, "material.specularSize");
	uniloc->material.displacementSize = glGetUniformLocation(program, "material.displacementSize");
	uniloc->material.normalsSize = glGetUniformLocation(program, "material.normalsSize");

// make an offset here so it doesnt start at 0 again after doing diffuse (no same unit pls)

	int offsetTex = 0;

	for (int i = 0; i < texDiffuseSize; i++) {
		uniloc->material.diffuse[i] = glGetUniformLocation(
			program,
			("material.diffuse[" + std::to_string(i) + "]").c_str()
		);

		glUniform1i(uniloc->material.diffuse[i], i);
	}
	offsetTex += texDiffuseSize;

	for (int i = 0; i < texSpecularSize; i++) {
		uniloc->material.specular[i] = glGetUniformLocation(
			program,
			("material.specular[" + std::to_string(i) + "]").c_str()
		);

		glUniform1i(uniloc->material.specular[i], offsetTex + i);
	}
	offsetTex += texSpecularSize;

	for (int i = 0; i < texDisplacementSize; i++) {
		uniloc->material.displacement[i] = glGetUniformLocation(
			program,
			("material.displacement[" + std::to_string(i) + "]").c_str()
		);

		glUniform1i(uniloc->material.displacement[i], offsetTex + i);

		uniloc->material.height_scale[i] = glGetUniformLocation(
			program, ("material.height_scale[" + std::to_string(i) + "]").c_str()
		);
	}
	offsetTex += texDisplacementSize;

	for (int i = 0; i < texNormalsSize; i++) {
		uniloc->material.normals[i] = glGetUniformLocation(
			program,
			("material.normals[" + std::to_string(i) + "]").c_str()
		);

		glUniform1i(uniloc->material.normals[i], offsetTex + i);
		//std::cout << "texture normals assigned at bind location " << offsetTex + i << '\n';
	}
	offsetTex += texNormalsSize;

	uniloc->material.shininess = glGetUniformLocation(program, "material.shininess");

	// pointlight
	uniloc->pointLightAmount = glGetUniformLocation(program, "pointLightAmount");

	for (int i = 0; i < MAX_POINT_LIGHT; i++) {
		decltype(uniloc->pointLight[i])& pointUniLoc = uniloc->pointLight[i];
		
		uniloc->far_plane_pointLight[i] = glGetUniformLocation(
			program,
			("far_plane_pointLight[" + std::to_string(i) + "]").c_str()
		);

		uniloc->pointLight_shadowmap[i] = glGetUniformLocation(
			program,
			("pointLight_shadowmap[" + std::to_string(i) + "]").c_str()
		);

		glUniform1i(uniloc->pointLight_shadowmap[i], offsetTex + i);

		//uniloc->pointLight_lightpos[i] = glGetUniformLocation(
		//	program,
		//	("pointLight_lightpos[" + std::to_string(i) + "]").c_str()
		//);

		pointUniLoc.diffuse = glGetUniformLocation(
			program,
			("pointLight[" + std::to_string(i) + "].diffuse").c_str()
		);

		pointUniLoc.specular = glGetUniformLocation(
			program,
			("pointLight[" + std::to_string(i) + "].specular").c_str()
		);

		pointUniLoc.constant = glGetUniformLocation(
			program,
			("pointLight[" + std::to_string(i) + "].constant").c_str()
		);

		pointUniLoc.linear = glGetUniformLocation(
			program,
			("pointLight[" + std::to_string(i) + "].linear").c_str()
		);

		pointUniLoc.quadratic = glGetUniformLocation(
			program,
			("pointLight[" + std::to_string(i) + "].quadratic").c_str()
		);

		pointUniLoc.color = glGetUniformLocation(
			program,
			("pointLight[" + std::to_string(i) + "].color").c_str()
		);

		pointUniLoc.position = glGetUniformLocation(
			program,
			("pointLight[" + std::to_string(i) + "].position").c_str()
		);
	}
	offsetTex += MAX_POINT_LIGHT;

	//int n = -1;
	//glGetUniformiv(program, uniloc->pointLight_shadowmap[0], &n);
	//std::cout << n << ", " << offsetTex << " ------------ pointLight_shadowmap[0]\n";

	// spotlight
	uniloc->spotLightAmount = glGetUniformLocation(program, "spotLightAmount");

	for (int i = 0; i < MAX_SPOT_LIGHT; i++) {
		decltype(uniloc->spotLight[i])& spotUniLoc = uniloc->spotLight[i];

		uniloc->spotLight_shadowmap[i] = glGetUniformLocation(
			program,
			("spotLight_shadowmap[" + std::to_string(i) + "]").c_str()
		);
		uniloc->spotLight_lightMatrix[i] = glGetUniformLocation(
			program,
			("spotLight_lightMatrix[" + std::to_string(i) + "]").c_str()
		);

		glUniform1i(uniloc->spotLight_shadowmap[i], offsetTex + i);

		spotUniLoc.diffuse = glGetUniformLocation(
			program,
			("spotLight[" + std::to_string(i) + "].diffuse").c_str()
		);

		spotUniLoc.specular = glGetUniformLocation(
			program,
			("spotLight[" + std::to_string(i) + "].specular").c_str()
		);

		spotUniLoc.constant = glGetUniformLocation(
			program,
			("spotLight[" + std::to_string(i) + "].constant").c_str()
		);

		spotUniLoc.linear = glGetUniformLocation(
			program,
			("spotLight[" + std::to_string(i) + "].linear").c_str()
		);

		spotUniLoc.quadratic = glGetUniformLocation(
			program,
			("spotLight[" + std::to_string(i) + "].quadratic").c_str()
		);

		spotUniLoc.color = glGetUniformLocation(
			program,
			("spotLight[" + std::to_string(i) + "].color").c_str()
		);

		spotUniLoc.position = glGetUniformLocation(
			program,
			("spotLight[" + std::to_string(i) + "].position").c_str()
		);

		spotUniLoc.direction = glGetUniformLocation(
			program,
			("spotLight[" + std::to_string(i) + "].direction").c_str()
		);

		spotUniLoc.cutOff = glGetUniformLocation(
			program,
			("spotLight[" + std::to_string(i) + "].cutOff").c_str()
		);

		spotUniLoc.outerCutOff = glGetUniformLocation(
			program,
			("spotLight[" + std::to_string(i) + "].outerCutOff").c_str()
		);
	}
}



void triangle::Draw() {
	uniLocSetValues();
	glBindVertexArray(triangleVAO);
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void square::Draw() {
	uniLocSetValues();
	glBindVertexArray(squareVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

// viewport sets in shadowFBO::shadowMap()
// shadowfbo set to be used in shadowFBO::shadowMap()
// program also used in shadowFBO::shadowMap()
void cube::DrawDepthShadow() {
	if (transparency >= 1.0f)
		return;

	//unsigned int program = programShadowMap.getID();
	//glUseProgram(program);

	//glViewport(0, 0, SHADOW_TEX_WH, SHADOW_TEX_WH);
	//glBindFramebuffer(GL_FRAMEBUFFER, shadowfbo->get_ID()); in shadowFBO::shadowMap()


	if (use_instancing) {
		glBindVertexArray(instance.VAO);
		glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, instance.instance_amount);
		//std::cout << "drawing\n";
	}
	else {
		glBindVertexArray(cubeVAO);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}

	glBindVertexArray(0);
	//glBindFramebuffer(GL_FRAMEBUFFER, FBO_ID);
	

}

// deprecated
/*void cube::DrawShadow(shadowFBO* shadowfbo, Renderer* rend) { // draw shadow and cube, pass the texture
	if (transparency >= 1.0f)
		return;

	glBindTexture(GL_TEXTURE_2D, shadowfbo->getDepth_ID());
	// draws the shadow inside shadowFBO

	//glBindFramebuffer(GL_FRAMEBUFFER, FBO_ID); already binded in shadowFBO::shadowMap
	// instancing bind fbo in shadowMap

	if (isInstancingInit) {
		glBindVertexArray(instance.VAO);
		glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, instance.instance_amount);
	}
	else {
		glBindVertexArray(cubeVAO);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}
	
	glBindVertexArray(0);
}*/

void cube::Draw() {
	if (transparency >= 1.0f)
		return;

	unsigned int program;
	programType pType;

	bool hasNormals = material.normals.size() > 0;

	if (getReflectiveness() > 0) {
		if (use_instancing) {
			//std::cout << "insreflect1\n";
			program = hasNormals ? 
				programNormal3dInstancingReflection.getID() 
				: program3dInstancingReflection.getID();

			pType = hasNormals ?
				programType::reflectiveInstancing3DimNormal
				: programType::reflectiveInstancing3Dim;
		}
		else {
			program = hasNormals ?
				programNormal3Dreflection.getID()
				: program3Dreflection.getID();

			pType = hasNormals ?
				programType::reflective3DimNormal
				: programType::reflective3Dim;
		}
	}
	else {
		if (use_instancing) {
			program = hasNormals ?
				programNormal3dInstancing.getID()
				: program3dInstancing.getID();

			pType = hasNormals ?
				programType::instancing3DimNormal
				: programType::instancing3Dim;
		}
		else {
			program = hasNormals ?
				programNormal3d.getID()
				: program3d.getID();

			pType = hasNormals ?
				programType::threeDimNormal
				: programType::threeDim;
			//std::cout << "3d program\n";
		}
	}

	if (use_instancing) {
		//std::cout << "bindinstancevao\n";
		glBindVertexArray(instance.VAO);
	}
	else {
		glBindVertexArray(cubeVAO);
	}

	if (highlight) { // find abstraction and short it out then implement it on meshes models too
		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);

		//std::cout << "aoujiwdoaijdwoajdjajwdjaowdjoajwd\n";
		uniLocSetValues(program, pType);

		if (use_instancing) {
			//std::cout << "draw123012301230\n";
			glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, instance.instance_amount);
		}
		else {
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		}

		glStencilMask(0x00);
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);

		glDepthFunc(GL_LEQUAL);

		highlightMode = true;
		uniLocSetValues(program, pType);
		//glBindVertexArray(cubeVAO);

		glActiveTexture(GL_TEXTURE21);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texSkybox);
		glActiveTexture(GL_TEXTURE0);

		if (transparency > 0) glDepthMask(GL_FALSE);
		if (use_instancing) {
			//std::cout << "drawinstancevao111\n";
			glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, instance.instance_amount);
		}
		else {
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		}

		if (transparency > 0) glDepthMask(GL_TRUE);

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

		highlightMode = false;

		glStencilMask(0xFF);

		glDisable(GL_STENCIL_TEST);
		glDepthFunc(GL_LESS);

		glBindVertexArray(0);

		return;
	}

	uniLocSetValues(program, pType);

	glActiveTexture(GL_TEXTURE21);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texSkybox);
	glActiveTexture(GL_TEXTURE0);

	if (transparency > 0) glDepthMask(GL_FALSE);
	if (use_instancing) {
		//std::cout << "drawinstancevao111\n";
		glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, instance.instance_amount);
	}
	else
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	if (transparency > 0) glDepthMask(GL_TRUE);

	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void cube::initInstancing() {
	glGenVertexArrays(1, &instance.VAO);
	glBindVertexArray(instance.VAO);

	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 11, nullptr);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 11, (void*)(3 * sizeof(float)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 11, (void*)(6 * sizeof(float)));
	
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	// instance (offsets)
	glGenBuffers(1, &instance.VBOoffsets);
	glBindBuffer(GL_ARRAY_BUFFER, instance.VBOoffsets);
	glBufferData(GL_ARRAY_BUFFER, instance.instance_amount * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);

	std::size_t vec4Size = sizeof(glm::vec4);

	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, nullptr);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(vec4Size));
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);

	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);
	glEnableVertexAttribArray(5);
	glEnableVertexAttribArray(6);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	isInstancingInit = true;
}