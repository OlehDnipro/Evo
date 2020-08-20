
/* * * * * * * * * * * * * Author's note * * * * * * * * * * * *\
*   _       _   _       _   _       _   _       _     _ _ _ _   *
*  |_|     |_| |_|     |_| |_|_   _|_| |_|     |_|  _|_|_|_|_|  *
*  |_|_ _ _|_| |_|     |_| |_|_|_|_|_| |_|     |_| |_|_ _ _     *
*  |_|_|_|_|_| |_|     |_| |_| |_| |_| |_|     |_|   |_|_|_|_   *
*  |_|     |_| |_|_ _ _|_| |_|     |_| |_|_ _ _|_|  _ _ _ _|_|  *
*  |_|     |_|   |_|_|_|   |_|     |_|   |_|_|_|   |_|_|_|_|    *
*                                                               *
*                     http://www.humus.name                     *
*                                                                *
* This file is a part of the work done by Humus. You are free to   *
* use the code in any way you like, modified, unmodified or copied   *
* into your own work. However, I expect you to respect these points:  *
*  - If you use this file and its contents unmodified, or use a major *
*    part of this file, please credit the author and leave this note. *
*  - For use in anything commercial, please request my approval.     *
*  - Share your work and ideas too as much as you can.             *
*                                                                *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#pragma once

#include "../Humus/Graphics/Renderer.h"
#include "../Humus/Math/Vector.h"
#include "../Humus/Util/ModelDae.h"
using namespace Dae;
#include <vector>
class ShadowMapCascade
{
public:
	ShadowMapCascade();
	~ShadowMapCascade();

	bool CreateResources(Device device);
	void DestroyResources(Device device);

	void SetMatrix(Context context, const float4x4 &matrix);
	//RootSignature GetRootSignature() const { return m_RootSig; }
	void Draw(Context context);

private:
	Dae::VertexLayout vertexLayout = Dae::VertexLayout({
										VERTEX_COMPONENT_POSITION,
										VERTEX_COMPONENT_UV,
										VERTEX_COMPONENT_COLOR,
										VERTEX_COMPONENT_NORMAL,
									 });
	std::vector<Dae::Model> m_Models;
	std::vector<Texture> m_Textures;
	
};
