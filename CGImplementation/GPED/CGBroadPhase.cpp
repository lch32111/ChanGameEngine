#include "CGBroadPhase.h"

CGProj::CGBroadPhase::CGBroadPhase()
{
	m_proxyCount = 0;

	m_pairCapacity = 16;
	m_pairCount = 0;
	m_pairBuffer = new cgPair[m_pairCapacity];

	m_moveCapacity = 16;
	m_moveCount = 0;
	m_moveBuffer = new int[m_moveCapacity];
}

CGProj::CGBroadPhase::~CGBroadPhase()
{
	delete[] m_moveBuffer;
	delete[] m_pairBuffer;
	m_moveBuffer = nullptr;
	m_pairBuffer = nullptr;
}

int CGProj::CGBroadPhase::CreateProxy(const GPED::c3AABB & aabb, void * userData)
{
	int proxyId = m_tree.CreateProxy(aabb, userData);
	++m_proxyCount;
	BufferMove(proxyId);
	return proxyId;
}

void CGProj::CGBroadPhase::DestroyProxy(int proxyId)
{
	UnBufferMove(proxyId);
	--m_proxyCount;
	m_tree.DestroyProxy(proxyId);
}

void CGProj::CGBroadPhase::UpdateProxy(int proxyId, const GPED::c3AABB & aabb, const glm::vec3 & displacement)
{
	bool buffer = m_tree.UpdateProxy(proxyId, aabb, displacement);
	if (buffer)
		BufferMove(proxyId);
}

void CGProj::CGBroadPhase::UpdateProxy(int proxyId, const GPED::c3AABB & aabb)
{
	bool buffer = m_tree.UpdateProxy(proxyId, aabb);
	if (buffer)
		BufferMove(proxyId);
}

// This is called from DynamicTrees::query when we are gathering pairs
bool CGProj::CGBroadPhase::QueryCallback(int proxyId)
{
	// A proxy cannot form a pair with itself
	if (proxyId == m_queryProxyId)
		return true;

	// Grow the pair buffer as needed.
	if (m_pairCount == m_pairCapacity)
	{
		cgPair* oldBuffer = m_pairBuffer;
		m_pairCapacity *= 2;
		m_pairBuffer = new cgPair[m_pairCapacity];
		memcpy(m_pairBuffer, oldBuffer, m_pairCount * sizeof(cgPair));
		delete[] oldBuffer;
		oldBuffer = nullptr;
	}

	m_pairBuffer[m_pairCount].proxyIdA = GPED::rMin(proxyId, m_queryProxyId);
	m_pairBuffer[m_pairCount].proxyIdB = GPED::rMax(proxyId, m_queryProxyId);
	++m_pairCount;

	return true;
}

void CGProj::CGBroadPhase::BufferMove(int proxyId)
{
	if (m_moveCount == m_moveCapacity)
	{
		int* oldBuffer = m_moveBuffer;
		m_moveCapacity *= 2;
		m_moveBuffer = new int[m_moveCapacity];
		memcpy(m_moveBuffer, oldBuffer, m_moveCount * sizeof(int));
		delete[] oldBuffer;
		oldBuffer = nullptr;
	}

	m_moveBuffer[m_moveCount] = proxyId;
	++m_moveCount;
}

void CGProj::CGBroadPhase::UnBufferMove(int proxyId)
{
	for (int i = 0; i < m_moveCount; ++i)
	{
		if (m_moveBuffer[i] == proxyId)
			m_moveBuffer[i] = e_nullProxy;
	}
}


void * CGProj::CGBroadPhase::GetUserData(int proxyId) const
{
	return m_tree.GetUserData(proxyId);
}

bool CGProj::CGBroadPhase::TestOverlap(int proxyIdA, int proxyIdB)
{
	const GPED::c3AABB& aabbA = m_tree.GetFatAABB(proxyIdA);
	const GPED::c3AABB& aabbB = m_tree.GetFatAABB(proxyIdB);
	return GPED::aabbOverlap(aabbA, aabbB);
}

int CGProj::CGBroadPhase::GetProxyCount() const
{
	return m_proxyCount;
}

void CGProj::BroadRendererWrapper::draw(Shader* shader, glm::mat4* proj, glm::mat4* view)
{
	m_shader = shader;
	m_projection = proj;
	m_view = view;

	shader->use();
	shader->setMat4("projection", *m_projection);
	shader->setMat4("view", *view);
	if (m_tree->GetHeight() >= 0)
		recursiveDraw(m_tree->m_root);
}

void CGProj::BroadRendererWrapper::recursiveDraw(int index)
{
	if (m_tree->m_nodes[index].isLeaf() == false)
		render(m_tree->m_nodes[index].aabb, branchColor, branchWidth);
	else
		render(m_tree->m_nodes[index].aabb, leafColor, leafWidth);

	int left = m_tree->m_nodes[index].left;
	int right = m_tree->m_nodes[index].right;

	if (left != Node_Null)
		recursiveDraw(left);

	if (right != Node_Null)
		recursiveDraw(right);
}

#include <Graphics/GLPrimitiveUtil.h>
void CGProj::BroadRendererWrapper::render(const GPED::c3AABB& aabb, const glm::vec3& Color, float lineWidth)
{
	glm::mat4 model = glm::mat4(1.0);
	glm::vec3 halfextents = (aabb.max - aabb.min) / 2.f;
	glm::vec3 pos = aabb.min + halfextents;
	model = glm::translate(model, pos);
	model = glm::scale(model, halfextents);
	m_shader->setMat4("model", model);
	m_shader->setVec3("Color", Color);
	wireRenderCube(lineWidth);
}