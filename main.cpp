#include "OpenglWindow.h"
#include "ShaderProgram.h"
#include "Mesh.h"
#include "OBJLoader.h"
#include "FixedFunctionMeshDraw.h"

using VertexType = Vertex<
	int,			VertexTexID,
	Math::Point2f,	VertexTexCoord,
	Math::Point3f,	VertexNormal,
	Math::Point4f,	VertexPosition
>;

int main (int argc, char const *argv[])
{
	/// Window
	OpenglWindow newWindow(500, 500, "Shaders Example");

	/// Shader
	ShaderProgram exampleShader = ShaderProgram({
		{GL_VERTEX_SHADER, "textureShader.vert"},
		{GL_FRAGMENT_SHADER, "textureShader.frag"}
	});

	exampleShader.setMatrix("projectionMatrix", Math::projection<float>(
			55.0, 1, 0.1, 1000));
	exampleShader.setMatrix("viewMatrix", Math::identity<4, float>());
	exampleShader.setMatrix("worldMatrix", Math::translation<float>(0, -10, -50));

	exampleShader.setVector("uColor", Math::Vec4f(1, 1, 1, 1));

	/// Mesh
	Mesh testMesh = OBJLoader<VertexType>().loadMesh("Obj/", "Lowpoly_tree_sample.obj");
	glEnable(GL_TEXTURE_2D);

	while (newWindow.active) {
		if (newWindow.handleInput())
			if (newWindow.keyboard.getKeyState(newWindow.keyboard.ESC))
				newWindow.requestClose();
		newWindow.focus();

		FixedFunctionMeshDraw().draw(testMesh, exampleShader);
		
		newWindow.swapBuffers();
	}
	return 0;
}