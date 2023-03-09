#pragma once
#include "TextureBase.h"
#include "ShaderBase.h"
#include <vector>

namespace reactphysics3d {
	class RigidBody;
}

namespace NCL {
	using namespace NCL::Rendering;

	class MeshGeometry;
	namespace CSC8503 {
		using namespace Maths;

		class RenderObject
		{
		public:
			RenderObject(reactphysics3d::RigidBody* rigidBody, const Vector3& scale, MeshGeometry* mesh, TextureBase* tex, ShaderBase* shader);
			~RenderObject();

			void SetDefaultTexture(TextureBase* t) {
				texture = t;
			}

			TextureBase* GetDefaultTexture() const {
				return texture;
			}

			MeshGeometry*	GetMesh() const {
				return mesh;
			}

			reactphysics3d::RigidBody* GetPhysicsObject() const {
				return rigidBody;
			}

			Vector3 GetScale() const {
				return scale;
			}

			ShaderBase*		GetShader() const {
				return shader;
			}

			void SetColour(const Vector4& c) {
				colour = c;
			}

			Vector4 GetColour() const {
				return colour;
			}

			void SetFrameMatrices(const std::vector <Matrix4>& fmatrices) {
				frameMatrices = fmatrices;
			}

			std::vector<Matrix4> GetFrameMatrices() const {
				return frameMatrices;
			}

		protected:
			MeshGeometry*	mesh;
			TextureBase*	texture;
			ShaderBase*		shader;
			reactphysics3d::RigidBody* rigidBody;
			Vector3         scale;
			Vector4			colour;
			std::vector <Matrix4 > frameMatrices;

		};
	}
}
