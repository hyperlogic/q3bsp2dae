#ifndef _QUADPATCH_H_
#define _QUADPATCH_H_

#include "q3bsp.h"
#include <vector>

// tessalate a q3 quadratic patch.
class QuadPatch
{
public:
    QuadPatch();
    ~QuadPatch();
    void setControlPoints(int uSize, int vSize, const drawVert_t* verts);
    void generateMesh(int subdivisions);

    const std::vector<drawVert_t>& getVerts() const { return m_verts; }
    const std::vector<int32_t>& getIndices() const { return m_indices; }

protected:
    void generateVertex(float u, float v, drawVert_t& vertOut) const;

    int idx(int u, int v) const
	{
        return (m_uSize * v) + u;
	}

    bool areRowsColinear() const;
    bool areColumnsColinear() const;
    bool areVertsColinear(const std::vector<drawVert_t>& verts) const;

    int m_uSize;
    int m_vSize;
	std::vector<drawVert_t>m_currentControlVerts;
    std::vector<drawVert_t>m_controlVerts;
    std::vector<drawVert_t>m_verts;
    std::vector<int>m_indices;
};

#endif
