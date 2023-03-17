#include "RenderObject.h"
#include <reactphysics3d/reactphysics3d.h>
#include "MeshGeometry.h"

using namespace NCL::CSC8503;
using namespace NCL;

RenderObject::RenderObject(reactphysics3d::RigidBody* rigidBody, const Vector3& scale, MeshGeometry* mesh, TextureBase* tex, ShaderBase* shader,Vector4 colour) {
	this->rigidBody = rigidBody;
	this->scale     = scale;
	this->mesh		= mesh;
	this->texture	= tex;
	this->shader	= shader;
	/*this->colour	= Vector4(1.0f, 1.0f, 1.0f, 1.0f);*/
	this->colour = colour;
}

RenderObject::~RenderObject() {

}