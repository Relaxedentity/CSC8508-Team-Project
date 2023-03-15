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

			std::vector<Vector4> GetPaintedPos() const {
				return paints;
			}

			void AddPaintPos(Vector3 pos,int colour) {
				paints.push_back(Vector4(pos,colour));
			}

			void SetPaintPos(int index, Vector3 pos, int colour) {
				paints[index] = Vector4(pos,colour);
			}

			void ClearPaintPos() {
				if (paints.size() >= 0) {
					paints.clear();
				}
			}

			void PaintSpray(Vector3 explosionPos,char paintColour) {
				int colourInt;
				if (paintColour == 'r')
					colourInt = 1;
				else if (paintColour == 'O')
					colourInt = 0;
				else
					colourInt = 2;


				if (paintColour == 'O') {
					ClearPaintPos();
				}
				else if (GetPaintedPos().size() <= paintMax - 1) {
					AddPaintPos(explosionPos, colourInt);
				}
				else {
					if (paintCount < GetPaintedPos().size() - 1) {
						SetPaintPos(paintCount, explosionPos,colourInt);
						paintCount++;
					}
					else
					{
						SetPaintPos(paintCount, explosionPos,colourInt);
						paintCount = 0;
					}
				}
			}

			void Clear() {
				for (int i = 0; i < paints.size(); i++)
				{
					paints[i] = Vector4(paints[i].x, paints[i].y, paints[i].z, 0);
				}
			}

		protected:
			int paintCount = 0;
			int paintMax = 50;
			std::vector<Vector4> paints;
			
			MeshGeometry*	mesh;
			TextureBase*	texture;
			ShaderBase*		shader;
			reactphysics3d::RigidBody* rigidBody;
			Vector3         scale;
			Vector4			colour;
		};
	}
}
