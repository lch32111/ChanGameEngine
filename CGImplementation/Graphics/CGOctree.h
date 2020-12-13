#pragma once
#ifndef __CG_OCTREE_H__
#define __CG_OCTREE_H__

/*
	https://github.com/brandonpelfrey/SimpleOctree/blob/master/Octree.h
	The link above is my reference.
	I'm typing the code and learning octree with it.
*/

#include <vector>
#include <Math/CGVector3.h>

namespace CGProj
{
	class CGOctree
	{
		Math::CGVector3<float> m_origin;
		Math::CGVector3<float> m_halfDimension;

		CGOctree* m_children[8];
		Math::CGVector3<float>* m_data;
	public:
		CGOctree(const Math::CGVector3<float>& origin, const Math::CGVector3<float>& halfDimension)
			: m_origin(origin), m_halfDimension(halfDimension), m_data(nullptr)
		{
			for (int i = 0; i < 8; ++i)
				m_children[i] = nullptr;
		}

		CGOctree(const CGOctree& copy)
			: m_origin(copy.m_origin), m_halfDimension(copy.m_halfDimension), m_data(copy.m_data)
		{ }

		~CGOctree()
		{
			for (int i = 0; i < 8; ++i)
				delete m_children[i];
		}

		int getOctantContainingPoint(const Math::CGVector3<float>& point) const
		{
			int oct = 0;
			if (point[0] >= m_origin[0]) oct |= 4;
			if (point[1] >= m_origin[1]) oct |= 2;
			if (point[2] >= m_origin[2]) oct |= 1;
			return oct;
		}

		bool isLeafNode() const
		{
			return m_children[0] == nullptr;
		}

		void insert(Math::CGVector3<float>* point)
		{
			if (isLeafNode())
			{
				if (m_data == nullptr)
				{
					m_data = point;
					return;
				}
				else
				{
					Math::CGVector3<float>* oldPoint = m_data;
					m_data = nullptr;

					for (int i = 0; i < 8; ++i)
					{
						Math::CGVector3<float> newOrigin = m_origin;
						newOrigin[0] += m_halfDimension[0] * (i & 4 ? .5f : -.5f);
						newOrigin[1] += m_halfDimension[1] * (i & 2 ? .5f : -.5f);
						newOrigin[2] += m_halfDimension[2] * (i & 1 ? .5f : -.5f);
						m_children[i] = new CGOctree(newOrigin, m_halfDimension * .5f);
					}

					m_children[getOctantContainingPoint(*oldPoint)]->insert(oldPoint);
					m_children[getOctantContainingPoint(*point)]->insert(point);
				}
			}
			else
			{
				m_children[getOctantContainingPoint(*point)]->insert(point);
			}
		}

		void getPointsInsideBox(const Math::CGVector3<float>& bMin, const Math::CGVector3<float>& bMax, std::vector<Math::CGVector3<float>*>& results)
		{
			if (isLeafNode())
			{
				if (m_data != nullptr)
				{
					const Math::CGVector3<float>& p = *m_data;

					if (p[0] > bMax[0] || p[1] > bMax[1] || p[2] > bMax[2]) return;
					if (p[0] < bMin[0] || p[1] < bMin[1] || p[2] < bMin[2]) return;
					results.push_back(m_data);
				}
				else
				{
					for (int i = 0; i < 8; ++i)
					{
						const Math::CGVector3<float> cMax = m_children[i]->m_origin + m_children[i]->m_halfDimension;
						const Math::CGVector3<float> cMin = m_children[i]->m_origin - m_children[i]->m_halfDimension;

						if (cMax[0] < bMin[0] || cMax[1] < bMin[1] || cMax[2] < bMin[2]) continue;
						if (cMin[0] > bMax[0] || cMin[1] > bMax[1] || cMin[2] > bMax[2]) continue;

						m_children[i]->getPointsInsideBox(bMin, bMax, results);
					}
				}
			}
		}

	};
}

#endif
