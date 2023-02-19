#pragma once
#include "TextureBase.h"
#include "ShaderBase.h"

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

			std::vector<Vector3> GetPaintedPos() const {
				return paints;
			}

			void AddPaintPos(Vector3 pos) {
				paints.push_back(pos);
			}

			void SetPaintPos(int index, Vector3 pos) {
				paints[index] = pos;
			}

			void PaintSpray(Vector3 explosionPos) {
				if (GetPaintedPos().size() <= paintMax - 1) {
					AddPaintPos(explosionPos);
				}
				else {
					if (paintCount < GetPaintedPos().size() - 1) {
						SetPaintPos(paintCount, explosionPos);
						paintCount++;
					}
					else
					{
						SetPaintPos(paintCount, explosionPos);
						paintCount = 0;
					}
				}
			}

		protected:
			MeshGeometry*	mesh;
			TextureBase*	texture;
			ShaderBase*		shader;
			reactphysics3d::RigidBody* rigidBody;
			Vector3         scale;
			Vector4			colour;
			std::vector<Vector3> paints;
			int paintCount = 0;
			int paintMax = 10;
		};
	}
}
