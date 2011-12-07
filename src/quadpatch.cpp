#include "quadpatch.h"
#include <math.h>
#include <assert.h>

// crappy math lib

static bool fuzzEqual(float a, float b);

struct Vector3
{
    Vector3() {}
    Vector3(const vec3_t pos) : x(pos[0]), y(pos[1]), z(pos[2]) {}
    Vector3(float xIn, float yIn, float zIn) : x(xIn), y(yIn), z(zIn) {}
    Vector3 operator+(const Vector3& rhs) const { return Vector3(x + rhs.x, y + rhs.y, z + rhs.z); }
    void operator+=(const Vector3& rhs) { x += rhs.x; y += rhs.y; z += rhs.z; }
    Vector3 operator-(const Vector3& rhs) const { return Vector3(x - rhs.x, y - rhs.y, z - rhs.z); }
    Vector3 operator*(float s) const { return Vector3(x * s, y * s, z * s); }
    bool fuzzEqual(const Vector3& rhs) const { return ::fuzzEqual(x, rhs.x) && ::fuzzEqual(y, rhs.y) && ::fuzzEqual(z, rhs.z); }
    float dot(const Vector3& rhs) const { return x * rhs.x + y * rhs.y + z * rhs.z; }
    float length() const { return sqrt(dot(*this)); }

    float x;
    float y;
    float z;
};

struct Vector2
{
    Vector2() {}
    Vector2(const float pos[2]) : x(pos[0]), y(pos[1]) {}
    Vector2(float xIn, float yIn) : x(xIn), y(yIn) {}
    Vector2 operator+(const Vector2& rhs) const { return Vector2(x + rhs.x, y + rhs.y); }
    void operator+=(const Vector2& rhs) { x += rhs.x; y += rhs.y; }
    Vector2 operator-(const Vector2& rhs) const { return Vector2(x - rhs.x, y - rhs.y); }
    Vector2 operator*(float s) const { return Vector2(x * s, y * s); }
    bool fuzzEqual(const Vector2& rhs) const { return ::fuzzEqual(x, rhs.x) && ::fuzzEqual(y, rhs.y); }
    float dot(const Vector2& rhs) const { return x * rhs.x + y * rhs.y; }
    float length() const { return sqrt(dot(*this)); }

    float x;
    float y;
};

static bool fuzzEqual(float a, float b)
{
    const float epsilon = 0.0001f;
    float d = a - b;
    return fabs(d) < epsilon;
}

static float basisFunc(int i, float u)
{
    if (i == 0)
        return (1.0f - u) * (1.0f - u);
    else if (i == 1)
        return 2.0f * (1.0f - u) * u;
    else
        return u * u;
}

QuadPatch::QuadPatch() :
    m_currentControlVerts(9)  // allocate 9 control points.
{
    ;
}

QuadPatch::~QuadPatch()
{
    ;
}

void QuadPatch::setControlPoints(int uSize, int vSize, const drawVert_t* verts)
{
    m_uSize = uSize;
    m_vSize = vSize;
    m_controlVerts.resize(uSize * vSize);
    for (int i = 0; i < (uSize * vSize); i++)
        m_controlVerts[i] = verts[i];
}

bool QuadPatch::areRowsColinear() const
{
	std::vector<drawVert_t> rowVec(m_uSize);
	for (int j = 0; j < m_vSize; ++j)
	{
		for (int i = 0; i < m_uSize; ++i)
		{
			rowVec[i] = m_controlVerts[idx(i, j)];
		}

		// return false if this row is not colinear.
		if (!areVertsColinear(rowVec))
			return false;
	}
	return true;
}

bool QuadPatch::areColumnsColinear() const
{
	std::vector<drawVert_t> columnVec(m_vSize);
	for (int i = 0; i < m_uSize; ++i)
	{
		for (int j = 0; j < m_vSize; ++j)
		{
			columnVec[j] = m_controlVerts[idx(i, j)];
		}

		// return false if this column is not colinear.
		if (!areVertsColinear(columnVec))
			return false;
	}
	return true;
}

bool QuadPatch::areVertsColinear(const std::vector<drawVert_t>& verts) const
{
	if (verts.size() == 0)
		return false;  //!?!? no verts
	if (verts.size() == 1)
		return true;  // one vert is colinear with itself, i guess.

	// L(t) = B + tM
	// Let D be the distance between a point P and the line L.
	// D = |P - (B + t0M)|, where t0 = M ( P - B ) / (M * M)
	// if D is 0, then then point P lies on line L.

	// build the line from the first vert an the last.
	Vector3 B(verts[0].xyz);
	Vector3 temp(verts[verts.size() - 1].xyz);
	Vector3 M = temp - B;

	if (!M.fuzzEqual(Vector3(0, 0, 0)))
	{
		// check all the intermediate verts to see if they lie on the line.
		for (unsigned int i = 1; i < verts.size() - 1; ++i)
		{
            Vector3 P(verts[i].xyz);
			float t0 = M.dot(P - B) / M.dot(M);
			Vector3 D = P - (B + (M * t0));

			if (!fuzzEqual(D.length(), 0.0f))
				return false;  // this vert does NOT lie on the line.
		}
	}
	else // instead of colinear, check to see if all the points are equal.
	{
		// check all the intermediate verts to see if they are equal to B.
		for (unsigned int i = 1; i < verts.size() - 1; ++i)
		{
            Vector3 P(verts[i].xyz);
			if (!B.fuzzEqual(P))
				return false;  // this vert does NOT equal the others, there for is not colinear.
		}
	}

	return true; // all verts lie on the line.
}

void QuadPatch::generateMesh(int subdivisions)
{
	// sub divide subdiv times in u and v.
	int patchWidth = (m_uSize - 1) / 2;
	int patchHeight = (m_vSize - 1) / 2;

	// for each patch
    int i, j;

	float uSubdivisions = subdivisions;
	float vSubdivisions = subdivisions;

	// check rows are colinear.
	if (areRowsColinear())
		uSubdivisions = 1;  // then don't subdivide u.

	// check columns are colinear.
	if (areColumnsColinear())
		vSubdivisions = 1;  // then don't subdivide v.

	float uDelta = 1.0f / uSubdivisions;
	float vDelta = 1.0f / vSubdivisions;

	float vStart, uStart;
	float u, v;

	for (j = 0; j < patchHeight; ++j)
	{
		if (j == 0)
			vStart = 0.0f;
		else
			vStart = vDelta;

		for (v = vStart; v <= 1.01; v += vDelta)
		{
			for (i = 0; i < patchWidth; ++i)
			{
				drawVert_t vert;

				// set up this control verts for the current patch
				m_currentControlVerts[0] = m_controlVerts[ idx( i*2 + 0, j*2 + 0 )];
				m_currentControlVerts[1] = m_controlVerts[ idx( i*2 + 1, j*2 + 0 )];
				m_currentControlVerts[2] = m_controlVerts[ idx( i*2 + 2, j*2 + 0 )];
				m_currentControlVerts[3] = m_controlVerts[ idx( i*2 + 0, j*2 + 1 )];
				m_currentControlVerts[4] = m_controlVerts[ idx( i*2 + 1, j*2 + 1 )];
				m_currentControlVerts[5] = m_controlVerts[ idx( i*2 + 2, j*2 + 1 )];
				m_currentControlVerts[6] = m_controlVerts[ idx( i*2 + 0, j*2 + 2 )];
				m_currentControlVerts[7] = m_controlVerts[ idx( i*2 + 1, j*2 + 2 )];
				m_currentControlVerts[8] = m_controlVerts[ idx( i*2 + 2, j*2 + 2 )];

				if (i == 0)
					uStart = 0.0f;
				else
					uStart = uDelta;

				for (u = uStart; u <= 1.01; u += uDelta)
				{
					generateVertex(u, v, vert);

					// store this vert.
					m_verts.push_back(vert);
				}
			}
		}
	}


	// figure out the meshIndices.
	//meshIndicies.resize ( ((patchWidth * subdivisions) + 1) * (patchHeight * subdivisions) * 6 );

	for ( j = 0; j < patchHeight * vSubdivisions; ++j )
	{
		for ( i = 0; i < patchWidth * uSubdivisions; ++i )
		{
			int a, b, c, d;
			a = (((patchWidth * uSubdivisions) + 1) * j) + i;
			b = a + 1;
			c = (((patchWidth * uSubdivisions) + 1) * (j + 1)) + i;
			d = c + 1;

            assert(a >= 0 && a < (int)m_verts.size());
            assert(b >= 0 && b < (int)m_verts.size());
            assert(c >= 0 && c < (int)m_verts.size());
            assert(d >= 0 && d < (int)m_verts.size());

			m_indices.push_back(a);
			m_indices.push_back(c);
			m_indices.push_back(b);
			m_indices.push_back(b);
			m_indices.push_back(c);
			m_indices.push_back(d);
		}
	}
}

void QuadPatch::generateVertex(float u, float v, drawVert_t& vertOut) const
{
	Vector3 sumPos(0, 0, 0);
	Vector2 sumUv0(0, 0);
	Vector2 sumUv1(0, 0);

	for (int j = 0; j < 3; ++j)
	{
		for (int i = 0; i < 3; ++i)
		{
			Vector3 pos(m_currentControlVerts[(j * 3) + i].xyz);
			Vector2 uv0(m_currentControlVerts[(j * 3) + i].st);
			Vector2 uv1(m_currentControlVerts[(j * 3) + i].lightmap);

			// TODO: Ignoring normal & color

            float b = basisFunc(i, u) * basisFunc(j, v);
            sumPos += pos * b;
            sumUv0 += uv0 * b;
            sumUv1 += uv1 * b;
		}
	}

    vertOut.xyz[0] = sumPos.x;
    vertOut.xyz[1] = sumPos.y;
    vertOut.xyz[2] = sumPos.z;
    vertOut.st[0] = sumUv0.x;
    vertOut.st[1] = sumUv0.y;
    vertOut.lightmap[0] = sumUv1.x;
    vertOut.lightmap[1] = sumUv1.y;

    // TODO: Ignoring normal & color
    vertOut.normal[0] = 1;
    vertOut.normal[1] = 0;
    vertOut.normal[2] = 0;
    vertOut.color[0] = 255;
    vertOut.color[1] = 255;
    vertOut.color[2] = 255;
    vertOut.color[3] = 255;
}
