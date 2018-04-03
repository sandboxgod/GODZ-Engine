
#include "Lightmap.h"
#include "ResourceManager.h"
#include <CoreFramework/Math/vectormath.h>

using namespace GODZ;

GODZ_API void GODZ::CreateLightmaps(int numpolys, Polygon* polylist, Lightmap* lightmaplist, StaticLight* staticlight, size_t numStaticLights)
{
	//lightmaps are 16x16
    const int Width = 16;
    const int Height = 16;
	Vector3 lumels[Width][Height];               // world coordinates of lumels
	Vector3 edge1, edge2;
	Vector3 newedge1, newedge2;
	Vector3 Vect1, Vect2;
	Vector3 UVVector;
    Vector3 poly_normal;
    Vector3 lightvector;
    Vector3 pointonplane;
    float lumelcolor[Width][Height][3];  // array to hold lumel rgb values
    float lightdistance;
	float uvMin_U;
	float uvMin_V;
	float uvMax_U;
	float uvMax_V;
	float cosAngle;
    float X, Y, Z;
    float Distance;
    float uvDelta_U;
    float uvDelta_V;
    float ufactor;
    float vfactor;
    float combinedred;
    float combinedgreen;
    float combinedblue;
    float intensity;
    int flag; // set to 1, 2 or 3 depending on whether normal's X, Y or Z component is largest
    //char temp[256];
    unsigned char* lightmap = new unsigned char[Width * Height * 4];

	for (int loop = 0; loop < numpolys; loop++)
	{
        // get uv's in 3D worldspace
        //poly_normal = polylist[loop].GetNormal();
        //poly_normal.Normalize();
		poly_normal = polylist[loop].plane.N;
        pointonplane.x = polylist[loop].Vertex[0].pos.x;
        pointonplane.y = polylist[loop].Vertex[0].pos.y;
        pointonplane.z = polylist[loop].Vertex[0].pos.z;

        // find plane to map onto
        if (fabs(poly_normal.x) > fabs(poly_normal.y) && fabs(poly_normal.x) > fabs(poly_normal.z))
        {
            flag = 1;
            polylist[loop].Vertex[0].lu = polylist[loop].Vertex[0].pos.y;
            polylist[loop].Vertex[0].lv = polylist[loop].Vertex[0].pos.z;
            polylist[loop].Vertex[1].lu = polylist[loop].Vertex[1].pos.y;
            polylist[loop].Vertex[1].lv = polylist[loop].Vertex[1].pos.z;
            polylist[loop].Vertex[2].lu = polylist[loop].Vertex[2].pos.y;
            polylist[loop].Vertex[2].lv = polylist[loop].Vertex[2].pos.z;
        }
        else if (fabsf(poly_normal.y) > fabsf(poly_normal.x) && fabsf(poly_normal.y) > fabsf(poly_normal.z))
        {
            flag = 2;
            polylist[loop].Vertex[0].lu = polylist[loop].Vertex[0].pos.x;
            polylist[loop].Vertex[0].lv = polylist[loop].Vertex[0].pos.z;
            polylist[loop].Vertex[1].lu = polylist[loop].Vertex[1].pos.x;
            polylist[loop].Vertex[1].lv = polylist[loop].Vertex[1].pos.z;
            polylist[loop].Vertex[2].lu = polylist[loop].Vertex[2].pos.x;
            polylist[loop].Vertex[2].lv = polylist[loop].Vertex[2].pos.z;
        }
        else
        {
            flag = 3;
            polylist[loop].Vertex[0].lu = polylist[loop].Vertex[0].pos.x;
            polylist[loop].Vertex[0].lv = polylist[loop].Vertex[0].pos.y;
            polylist[loop].Vertex[1].lu = polylist[loop].Vertex[1].pos.x;
            polylist[loop].Vertex[1].lv = polylist[loop].Vertex[1].pos.y;
            polylist[loop].Vertex[2].lu = polylist[loop].Vertex[2].pos.x;
            polylist[loop].Vertex[2].lv = polylist[loop].Vertex[2].pos.y;
        }

        // convert to 2D texture space
        uvMin_U = polylist[loop].Vertex[0].lu;
        uvMin_V = polylist[loop].Vertex[0].lv;
        uvMax_U = polylist[loop].Vertex[0].lu;
        uvMax_V = polylist[loop].Vertex[0].lv;

        for (int i = 0; i < 3; i++)
        {
            if (polylist[loop].Vertex[i].lu < uvMin_U )
                uvMin_U = polylist[loop].Vertex[i].lu;

            if (polylist[loop].Vertex[i].lv < uvMin_V )
                uvMin_V = polylist[loop].Vertex[i].lv;

            if (polylist[loop].Vertex[i].lu > uvMax_U )
                uvMax_U = polylist[loop].Vertex[i].lu;

	        if (polylist[loop].Vertex[i].lv > uvMax_V )
                uvMax_V = polylist[loop].Vertex[i].lv;
        }

        uvDelta_U = uvMax_U - uvMin_U;
        uvDelta_V = uvMax_V - uvMin_V;

        for (int i = 0; i < 3; i++)
        {
            polylist[loop].Vertex[i].lu -= uvMin_U;
            polylist[loop].Vertex[i].lv -= uvMin_V;
            polylist[loop].Vertex[i].lu /= uvDelta_U;
            polylist[loop].Vertex[i].lv /= uvDelta_V;
        }

        // calculate the world space position for each lumel
        Distance = - (poly_normal.x * pointonplane.x + poly_normal.y * pointonplane.y + poly_normal.z * pointonplane.z);

	    switch (flag)
        {
            case 1: //YZ Plane
                X = - ( poly_normal.y * uvMin_U + poly_normal.z * uvMin_V + Distance ) / poly_normal.x;
                UVVector.x = X;
                UVVector.y = uvMin_U;
                UVVector.z = uvMin_V;
                X = - ( poly_normal.y * uvMax_U + poly_normal.z * uvMin_V + Distance ) / poly_normal.x;
                Vect1.x = X;
                Vect1.y = uvMax_U;
                Vect1.z = uvMin_V;
                X = - ( poly_normal.y * uvMin_U + poly_normal.z * uvMax_V + Distance ) / poly_normal.x;
                Vect2.x = X;
                Vect2.y = uvMin_U;
                Vect2.z = uvMax_V;
            break;

            case 2: //XZ Plane
                Y = - ( poly_normal.x * uvMin_U + poly_normal.z * uvMin_V + Distance ) / poly_normal.y;
                UVVector.x = uvMin_U;
                UVVector.y = Y;
                UVVector.z = uvMin_V;
                Y = - ( poly_normal.x * uvMax_U + poly_normal.z * uvMin_V + Distance ) / poly_normal.y;
                Vect1.x = uvMax_U;
                Vect1.y = Y;
                Vect1.z = uvMin_V;
                Y = - ( poly_normal.x * uvMin_U + poly_normal.z * uvMax_V + Distance ) / poly_normal.y;
                Vect2.x = uvMin_U;
                Vect2.y = Y;
                Vect2.z = uvMax_V;
            break;

            case 3: //XY Plane
                Z = - ( poly_normal.x * uvMin_U + poly_normal.y * uvMin_V + Distance ) / poly_normal.z;
                UVVector.x = uvMin_U;
                UVVector.y = uvMin_V;
                UVVector.z = Z;
                Z = - ( poly_normal.x * uvMax_U + poly_normal.y * uvMin_V + Distance ) / poly_normal.z;
                Vect1.x = uvMax_U;
                Vect1.y = uvMin_V;
                Vect1.z = Z;
                Z = - ( poly_normal.x * uvMin_U + poly_normal.y * uvMax_V + Distance ) / poly_normal.z;
                Vect2.x = uvMin_U;
                Vect2.y = uvMax_V;
                Vect2.z = Z;
            break;
        }

        edge1.x = Vect1.x - UVVector.x;
        edge1.y = Vect1.y - UVVector.y;
        edge1.z = Vect1.z - UVVector.z;
        edge2.x = Vect2.x - UVVector.x;
        edge2.y = Vect2.y - UVVector.y;
        edge2.z = Vect2.z - UVVector.z;
        for(int iX = 0; iX < Width; iX++)
        {
            for(int iY = 0; iY < Height; iY++)
            {
                ufactor = (iX / (float)Width);
                vfactor = (iY / (float)Height);
                newedge1.x = edge1.x * ufactor;
                newedge1.y = edge1.y * ufactor;
                newedge1.z = edge1.z * ufactor;
                newedge2.x = edge2.x * vfactor;
                newedge2.y = edge2.y * vfactor;
                newedge2.z = edge2.z * vfactor;

                lumels[iX][iY].x = UVVector.x + newedge2.x + newedge1.x;
                lumels[iX][iY].y = UVVector.y + newedge2.y + newedge1.y;
                lumels[iX][iY].z = UVVector.z + newedge2.z + newedge1.z;

                combinedred = 0.0;
                combinedgreen = 0.0;
                combinedblue = 0.0;
                for (size_t i = 0; i < numStaticLights; i++)
                {
                    if (classifyPoint(staticlight[i].Position, pointonplane, poly_normal) != PLANE_BACKSIDE)
                    {
                        lightvector.x = staticlight[i].Position.x - lumels[iX][iY].x;
                        lightvector.y = staticlight[i].Position.y - lumels[iX][iY].y;
                        lightvector.z = staticlight[i].Position.z - lumels[iX][iY].z;
                        lightdistance = lightvector.GetLength();
                        lightvector.Normalize();
                        cosAngle = poly_normal.Dot(lightvector);
                        if (lightdistance < staticlight[i].Radius)
                        {
                            intensity = (staticlight[i].Brightness * cosAngle) / lightdistance;
                            combinedred += staticlight[i].color.r * intensity;
                            combinedgreen += staticlight[i].color.g * intensity;
                            combinedblue += staticlight[i].color.b * intensity;
                        }
                    }
                }
                if (combinedred > 255.0)
                    combinedred = 255.0;
                if (combinedgreen > 255.0)
                    combinedgreen = 255.0;
                if (combinedblue > 255.0)
                    combinedblue = 255.0;
                lumelcolor[iX][iY][0] = combinedred;
                lumelcolor[iX][iY][1] = combinedgreen;
                lumelcolor[iX][iY][2] = combinedblue;
            }
        }

        for(int iX = 0; iX < (Width * 4); iX += 4)
        {
            for(int iY = 0; iY < Height; iY += 1)
            {
                lightmap[iX + iY * Height * 4] = (char)lumelcolor[iX / 4][iY][0];         // Red
                lightmap[iX + iY * Height * 4 + 1] = (char)lumelcolor[iX / 4][iY][1];     // Green
                lightmap[iX + iY * Height * 4 + 2] = (char)lumelcolor[iX / 4][iY][2];     // Blue
                lightmap[iX + iY * Height * 4 + 3] = 255;                                 // Alpha
            }
        }

    	//sprintf(lightmaplist[loop].m_tex.TexName, "%s", AppDirectory);
        //strcat(lightmaplist[loop].m_tex.TexName, "\\images\\lm");
        //strcat(lightmaplist[loop].m_tex.TexName, itoa(numLightmaps++, temp, 10));
        //strcat(lightmaplist[loop].m_tex.TexName, ".tga");
        //tgaSave(lightmaplist[loop].Texture.TexName, Width, Height, 32, lightmap);

    }
    delete[] lightmap;
    return;
}