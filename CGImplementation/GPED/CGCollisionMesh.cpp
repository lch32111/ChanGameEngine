#include <CGPrecompiled.h>
#include <GPED/CGCollisionMesh.h>
#include <GPED/CGPhysicsUtil.h>
#include <iostream>

CG::CGCollisionMesh::CGCollisionMesh()
{
	CGCollisionPrimitive::m_primitiveType = COLLISION_PRIMITIVE_MESH;
}

void
CG::CGCollisionMesh::getQuantizedGridPoints(
	const GPED::c3AABB & aabbFromDynamicObjects, Mesh_QuantizedGridRange& outRange) const
{
	// Mesh Setting
	glm::vec3 meshMin(m_meshWidth * -0.5f, -100.f, m_meshDepth * -0.5f);
	glm::vec3 meshMax(m_meshWidth * 0.5f, 100.f, m_meshDepth * 0.5f);

	// clamp to prevent the indices from going beyond the original range of the mesh.
	glm::vec3 AABBmin, AABBmax;
	AABBmin = GPED::rMax(meshMin, aabbFromDynamicObjects.min);
	AABBmax = GPED::rMin(meshMax, aabbFromDynamicObjects.max);

	// TODO : I just assume there is no rotation on the mesh. So, I have to handle with this problem later.
	// This Vector is used for translating mesh position, ranging 0 ~ m_meshWidth
	// That will make it easy to construct indices for the mesh
	glm::vec3 translateForEase(m_meshWidth * 0.5, 0, m_meshDepth * 0.5);
	AABBmin += translateForEase;
	AABBmax += translateForEase;

	float AllScaler = 1.f / ((float)m_meshWidth / (float)m_meshSubWidth);
	AABBmin *= AllScaler;
	AABBmax *= AllScaler;

	// Quantize!
	int quantizedAABBmin[3];
	int quantizedAABBmax[3];
	for (unsigned i = 0; i < 3; ++i)
	{
		// expand min/max quantized values for edge cases
		quantizedAABBmin[i] = getQuantized(AABBmin[i]) - 1;
		quantizedAABBmax[i] = getQuantized(AABBmax[i]) + 1;
	}

	outRange.startX = 0;
	outRange.EndX = m_meshSubWidth;
	outRange.startZ = 0;
	outRange.EndZ = m_meshSubDepth;
	if (quantizedAABBmin[0] > outRange.startX)
		outRange.startX = quantizedAABBmin[0];
	if (quantizedAABBmax[0] < outRange.EndX)
		outRange.EndX = quantizedAABBmax[0];
	if (quantizedAABBmin[2] > outRange.startZ)
		outRange.startZ = quantizedAABBmin[2];
	if (quantizedAABBmax[2] < outRange.EndZ)
		outRange.EndZ = quantizedAABBmax[2];
}

void CG::CGCollisionMesh::getVertex(unsigned gridX, unsigned gridZ, glm::vec3 & outVertex) const
{
	outVertex.x = m_meshWidth * (gridX * (1.f / m_meshSubWidth) - 0.5f);
	outVertex.z = m_meshDepth * (gridZ * (1.f / m_meshSubDepth) - 0.5f);
	outVertex.y = m_heightData[gridX + gridZ * (m_meshSubWidth + 1)];

	outVertex += m_meshOrigin;
}

int CG::CGCollisionMesh::getQuantized(float n) const
{
	if (n < 0.0)
	{
		return (int)(n - 0.5);
	}
	return (int)(n + 0.5);
}
