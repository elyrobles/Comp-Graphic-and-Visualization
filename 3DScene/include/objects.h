#pragma once
#include <glm/glm.hpp>
#include <vector>

struct Vertex {
    glm::vec3 Position{ 0.f, 0.f, 0.f };
    glm::vec3 Color{ 1.f, 1.f, 1.f };
    glm::vec3 Normal{ 0.f, 0.f, 0.f };
    glm::vec2 Uv{ 1.f, 1.f };  
};

// structure for my 3D makeup item of a sphere and cylinder attached together
struct Shapes {
    // vertices for the sphere
    // Sphere UV Mapping and normal lighting
    static std::vector<Vertex> makeSphereVertices(float radius, int segments) {
        std::vector<Vertex> vertices;
        float vOffset = 0.25f; // silver part is the top 25% of the texture image

        for (int y = 0; y <= segments; ++y) {
            for (int x = 0; x <= segments; ++x) {
                float xSection = static_cast<float>(x) / segments;
                float ySection = static_cast<float>(y) / segments;

                // spherical coordinates calculation
                float xCoordinate = std::cos(xSection * 2.0f * 3.1415926f) * std::sin(ySection * 3.1415926f);
                float yCoordinate = std::cos(ySection * 3.1415926f);
                float zCoordinate = std::sin(xSection * 2.0f * 3.1415926f) * std::sin(ySection * 3.1415926f);

                Vertex vertex;
                vertex.Position = glm::vec3(xCoordinate, yCoordinate, zCoordinate) * radius;
                vertex.Color = glm::vec3(0.75f, 0.75f, 0.75f);

                // texture coordinates for the sphere 
                vertex.Uv = glm::vec2(xSection, ySection * vOffset);

                // Normal calculation for lighting
                vertex.Normal = glm::normalize(glm::vec3(xCoordinate, yCoordinate, zCoordinate));

                vertices.push_back(vertex);
            }
        }
        return vertices;
    }
    // indices for drawing the sphere with triangles
    static std::vector<uint32_t> makeSphereIndices(int segments) {
        std::vector<uint32_t> indices;
        //calculates all indices of the triangles in sphere
        for (int y = 0; y < segments; ++y) {
            for (int x = 0; x < segments; ++x) {
                // for the corners of segments
                uint32_t topLeft = (y + 1) * (segments + 1) + x;
                uint32_t bottomLeft = y * (segments + 1) + x;
                uint32_t topRight = topLeft + 1;
                uint32_t bottomRight = bottomLeft + 1;

                // Creates 6 trianlges or indices for all segments
                indices.push_back(topLeft);
                indices.push_back(bottomLeft);
                indices.push_back(topRight);
                indices.push_back(bottomLeft);
                indices.push_back(bottomRight);
                indices.push_back(topRight);
            }
        }
        return indices;
    }
    // Cylinder UV Mapping and normal calculations
    static std::vector<Vertex> makeCylinderVertices(float radius, float height, int segments) {
        std::vector<Vertex> vertices;
        float vOffset = 0.25f; // Top 25% of the texture is silver portion of the photo

        for (int i = 0; i <= segments; ++i) {
            float angle = static_cast<float>(i) / segments * 2.0f * 3.1415926f;
            float u = static_cast<float>(i) / segments; // Normalize angle for U coordinate

            // Top edge of the cylinder
            float vTop = vOffset; // Top edge uses the silver part
            vertices.push_back({
                glm::vec3(radius * std::cos(angle), height / 2, radius * std::sin(angle)),
                glm::vec3(0.9f, 0.5f, 0.55f),
                // Normal calculation for the cylinder sides
                glm::normalize(glm::vec3(std::cos(angle), 0, std::sin(angle))), 
                glm::vec2(u, vTop)
                });

            // Bottom edge of the cylinder
            float vBottom = 1.0f; // Bottom edge uses the pink/ glass part of the photo
            vertices.push_back({
                glm::vec3(radius * std::cos(angle), -height / 2, radius * std::sin(angle)),
                glm::vec3(0.9f, 0.5f, 0.55f),
                // similar normal calculation as the top edge for consistent lighting across the cylinders surface
                glm::normalize(glm::vec3(std::cos(angle), 0, std::sin(angle))), 
                glm::vec2(u, vBottom)
                });
        }
        return vertices;
    }

    // calculates the indices for creating the cylinder with triangles
    static std::vector<uint32_t> makeCylinderIndices(int segments) {
        std::vector<uint32_t> indices;
        // this calculates the indices for all triangles that make up the cylinder
        for (int i = 0; i < segments; ++i) {
            int next = (i + 1) % (segments + 1);

            // indices for the top of cirlce
            indices.push_back(i);
            indices.push_back(next);
            indices.push_back(segments);

            // indices for the bottom of circle
            indices.push_back(segments + 1 + i);
            indices.push_back(segments + 1 + next);
            indices.push_back(2 * segments + 1);

            // indices for the sides of the cylinder
            indices.push_back(i);
            indices.push_back(next);
            indices.push_back(segments + 1 + next);
            indices.push_back(i);
            indices.push_back(segments + 1 + next);
            indices.push_back(segments + 1 + i);
        }
        return indices;
    }
    // This combines both the vertices of the cylinder and sphere to create one object
    static std::vector<Vertex> conjoinBothVertices(float cylinderRadius, float cylinderHeight, int cylinderSegments, float sphereRadius, int sphereSegments) {
        //vertices for sphere and cylinder
        std::vector<Vertex> bothVertices = makeCylinderVertices(cylinderRadius, cylinderHeight, cylinderSegments);
        std::vector<Vertex> sphereVertices = makeSphereVertices(sphereRadius, sphereSegments);
        // translation for sphere to go on top of cyliner
        glm::vec3 translation(0.0f, cylinderHeight / 2.0f + sphereRadius * 0.5f, 0.0f); 
        for (auto& vertex : sphereVertices) { 
            vertex.Position += translation;
            bothVertices.push_back(vertex);
        }

        return bothVertices;
    }
    // This combines the indices of the cylinder and sphre to create a single object
    static std::vector<uint32_t> conjoinBothIndices(int cylinderSegments, int sphereSegments) {
        //creates the indices for the sphere and cylinder
        std::vector<uint32_t> bothIndices = makeCylinderIndices(cylinderSegments);
        std::vector<uint32_t> sphereIndices = makeSphereIndices(sphereSegments);

        // align both sphere and cylinder indices
        uint32_t offset = static_cast<uint32_t>((cylinderSegments + 1) * 2);
        for (uint32_t index : sphereIndices) {
            bothIndices.push_back(index + offset);
        }

        return bothIndices;
    }

    // vertices and indices for combined cylinder and sphere
    static inline std::vector<Vertex> bothVertices = conjoinBothVertices(0.1f, 1.0f, 32, 0.15f, 32);
    static inline std::vector<uint32_t> bothIndices = conjoinBothIndices(32, 32);
};

// Second structure for the plane in which my objects will sit on
struct ShapesTwo {
    static inline std::vector<Vertex> planeVertices{
        // Triangle 1
        {
            .Position = {-5.0f, -0.5f,  5.0f}, // Top left
            .Color = {0.7f, 0.5f, 0.3f},
            .Normal = {0.f, 1.f, 0.f}, // lighting calculations for the plane
            .Uv = {0.0f, 0.0f} //Texturing the triangle vertices of the plane to match my wooden tiles photo
        },
        {
            .Position = { 5.0f, -0.5f,  5.0f}, // Top right
            .Color = {0.7f, 0.5f, 0.3f},
            .Normal = {0.f, 1.f, 0.f},
            .Uv = {1.0f, 0.0f}
        },
        {
            .Position = { 5.0f, -0.5f, -5.0f}, // Bottom right
            .Color = {0.7f, 0.5f, 0.3f},
            .Normal = {0.f, 1.f, 0.f},
            .Uv = {1.0f, 1.0f}
        },
        // Triangle 2
        {
            .Position = { 5.0f, -0.5f, -5.0f}, // Bottom right
            .Color = {0.7f, 0.5f, 0.3f},
            .Normal = {0.f, 1.f, 0.f},
            .Uv = {1.0f, 1.0f}
        },
        {
            .Position = {-5.0f, -0.5f, -5.0f}, // Bottom left
            .Color = {0.7f, 0.5f, 0.3f},
            .Normal = {0.f, 1.f, 0.f},
            .Uv = {0.0f, 1.0f}
        },
        {
            .Position = {-5.0f, -0.5f,  5.0f}, // Top left
            .Color = {0.7f, 0.5f, 0.3f},
            .Normal = {0.f, 1.f, 0.f},
            .Uv = {0.0f, 0.0f}
        }
    };

    // order in which triangles will be drawn
    static inline std::vector<uint32_t> planeIndices{
        0, 1, 2, // first triangle
        3, 4, 5  // second triangle
    };
};

struct ShapesThree {
    // Contains the vertices and indices that makes up the 3D quartz pyramid
    static inline std::vector<Vertex> pyramidVertices{
        //Front facing triangle
        {
            .Position = {-0.5f, -0.5f, 0.5f}, //bottom left
            .Color = {1.f, 0.5f, 0.5f},
            .Normal = {0.0f, 0.0f, 1.0f}, // for lighting calculations on each side of the pyramid
            .Uv = {0.2f, 0.7f} // texture for all sides of the pyramid
        },

        {
            .Position = {0.5f, -0.5f, 0.5f}, // bottom right
            .Color = {0.5f, 1.f, 0.5f},
            .Normal = {0.0f, 0.0f, 1.0f},
            .Uv = {0.7f, 0.7f}
        },

        {
            .Position = {0.0f, 0.5f, 0.0f}, // top point of pyramid
            .Color = {1.f, 0.f, 1.f},
            .Normal = {0.0f, 0.0f, 1.0f},
            .Uv = {0.5f, 0.2f}
        },

        //Right side of triangle
        {
            .Position = {0.5f, -0.5f, 0.5f}, // bottom right in the front
            .Color = {0.5f, 1.f, 0.5f},
            .Normal = {0.5f, 0.5f, 0.0f},
            .Uv = {0.7f, 0.7f}
        },

        {
            .Position = {0.5f, -0.5f, -0.5f}, // bottom right in the back
            .Color = {0.5f, 0.5f, 1.f},
            .Normal = {0.5f, 0.5f, 0.0f},
            .Uv = {1.0f, 0.7f}
        },

        {
            .Position = {0.0f, 0.5f, 0.0f}, // top point of pyramid (same as above, repeating anyway to avoid confusion)
            .Color = {1.f, 0.f, 1.f},
            .Normal = {0.5f, 0.5f, 0.0f},
            .Uv = {0.8, 0.2f}
        },

        // Back face of pyramid

        {
            .Position = {0.5f, -0.5f, -0.5f},  // bottom right
            .Color = {0.5f, 0.5f, 1.f},
            .Normal = {0.0f, 0.0f, -1.0f},
            .Uv = {0.7f, 0.2f}
        },

        {
            .Position = {-0.5f, -0.5f, -0.5f}, // bottom left
            .Color = {1.f, 1.f, 0.5f},
            .Normal = {0.0f, 0.0f, -1.0f},
            .Uv = {0.5f, 0.2f}
        },

        {
            .Position = {0.0f, 0.5f, 0.0f}, // top point of pyramid
            .Color = {1.f, 0.f, 1.f},
            .Normal = {0.0f, 0.0f, -1.0f},
            .Uv = {0.6f, 0.0f}
        },

        // Left face of pyramid

        {
            .Position = {-0.5f, -0.5f, -0.5f},  // bottom left in theback
            .Color = {1.f, 1.f, 0.5f},
            .Normal = {-0.5f, 0.5f, 0.0f},
            .Uv = {0.2f, 0.2f}
        },

        {
            .Position = {-0.5f, -0.5f, 0.5f}, // bottom left in the front
            .Color = {1.f, 0.5f, 0.5f},
            .Normal = {-0.5f, 0.5f, 0.0f},
            .Uv = {0.5f, 0.2f}
        },

        {
            .Position = {0.0f, 0.5f, 0.0f}, // top point of pyramid
            .Color = {1.f, 0.f, 1.f},
            .Normal = {-0.5f, 0.5f, 0.0f},
            .Uv = {0.3, 0.0f}
        },

        // Bottom of pyramid

        {
            .Position = {-0.5f, -0.5f, 0.5f},  // bottom left
            .Color = {1.f, 0.f, 0.f},
            .Normal = {0.0f, -1.0f, 0.0f},
            .Uv = {0.0f, 1.0f}
        },

        {
            .Position = {0.5f, -0.5f, 0.5f}, // bottom right
            .Color = {0.f, 1.f, 0.f},
            .Normal = {0.0f, -1.0f, 0.0f},
            .Uv = {1.0f, 1.0f}
        },

        {
            .Position = {0.5f, -0.5f, -0.5f}, // top right
            .Color = {1.f, 0.f, 1.f},
            .Normal = {0.0f, -1.0f, 0.0f},
            .Uv = {1.0f, 0.0f}
        },
        {
            .Position = {-0.5f, -0.5f, -0.5f}, // top left
            .Color = {1.f, 1.f, 0.f},
            .Normal = {0.0f, -1.0f, 0.0f},
            .Uv = {0.0f, 0.0f}
        },
    };


    // Order in which pyramid is drawn
    static inline std::vector<uint32_t> pyramidElements{
        0, 1, 2,   // front face of pyramid
        3, 4, 5,   // Right face of pyramid
        6, 7, 8,    // Back face of pyramid
        9, 10, 11, // Left face of pyramid

        12, 13, 14, // Bottom of Pyramid
        14, 15, 12
    };
};

// Vertices for creating an elongated cube to represent the purple sponge in my photo.
struct ShapesFour {
    static inline std::vector<Vertex> cubeVertices{
        //Front face
        {
            .Position = {-1.0f, 0.25f, 0.75f},
            .Color = {0.7f, 0.5f, 0.7f},
            .Normal = {0.0f, 0.0f, 1.0f},
            .Uv = {0.0f, 1.0f}
        },
        {
            .Position = {-1.0f, -0.25f, 0.75f},
            .Color = {0.7f, 0.5f, 0.7f},
            .Normal = {0.0f, 0.0f, 1.0f},
            .Uv = {0.0f, 0.0f}
        },
        {
            .Position = {1.0f, -0.25f, 0.75f},
            .Color = {0.7f, 0.5f, 0.7f},
            .Normal = {0.0f, 0.0f, 1.0f},
            .Uv = {1.0f, 0.0f}
        },
        {
            .Position = {1.0f, 0.25f, 0.75f},
            .Color = {0.7f, 0.5f, 0.7f},
            .Normal = {0.0f, 0.0f, 1.0f},
            .Uv = {1.0f, 1.0f}
        },

        //Right Face
        {
            .Position = {1.0f, 0.25f, 0.75f},
            .Color = {0.7f, 0.5f, 0.7f},
            .Normal = {1.0f, 0.0f, 0.0f},
            .Uv = {0.0f, 1.0f}
        },
        {
            .Position = {1.0f, -0.25f, 0.75f},
            .Color = {0.7f, 0.5f, 0.7f},
            .Normal = {1.0f, 0.0f, 0.0f},
            .Uv = {0.0f, 0.0f}
        },
        {
            .Position = {1.0f, -0.25f, -0.75f},
            .Color = {0.7f, 0.5f, 0.7f},
            .Normal = {1.0f, 0.0f, 0.0f},
            .Uv = {1.0f, 0.0f}
        },
        {
            .Position = {1.0f, 0.25f, -0.75f},
            .Color = {0.7f, 0.5f, 0.7f},
            .Normal = {1.0f, 0.0f, 0.0f},
            .Uv = {1.0f, 1.0f}
        },

        //Back Face
        {
            .Position = {1.0f, 0.25f, -0.75f},
            .Color = {0.7f, 0.5f, 0.7f},
            .Normal = {0.0f, 0.0f, -1.0f},
            .Uv = {1.0f, 1.0f}
        },
        {
            .Position = {1.0f, -0.25f, -0.75f},
            .Color = {0.7f, 0.5f, 0.7f},
            .Normal = {0.0f, 0.0f, -1.0f},
            .Uv = {1.0f, 0.0f}
        },
        {
            .Position = {-1.0f, -0.25f, -0.75f},
            .Color = {0.7f, 0.5f, 0.7f},
            .Normal = {0.0f, 0.0f, -1.0f},
            .Uv = {0.0f, 0.0f}
        },
        {
            .Position = {-1.0f, 0.25f, -0.75f},
            .Color = {0.7f, 0.5f, 0.7f},
            .Normal = {0.0f, 0.0f, -1.0f},
            .Uv = {0.0f, 1.0f}
        },

        //Left Face
        {
            .Position = {-1.0f, 0.25f, -0.75f},
            .Color = {0.7f, 0.5f, 0.7f},
            .Normal = {-1.0f, 0.0f, 0.0f},
            .Uv = {1.0f, 1.0f}
        },
        {
            .Position = {-1.0f, -0.25f, -0.75f},
            .Color = {0.7f, 0.5f, 0.7f},
            .Normal = {-1.0f, 0.0f, 0.0f},
            .Uv = {1.0f, 0.0f}
        },
        {
            .Position = {-1.0f, -0.25f, 0.75f},
            .Color = {0.7f, 0.5f, 0.7f},
            .Normal = {-1.0f, 0.0f, 0.0f},
            .Uv = {0.0f, 0.0f}
        },
        {
            .Position = {-1.0f, 0.25f, 0.75f},
            .Color = {0.7f, 0.5f, 0.7f},
            .Normal = {-1.0f, 0.0f, 0.0f},
            .Uv = {0.0f, 1.0f}
        },

        //Top Face
        {
            .Position = {-1.0f, 0.25f, -0.75f},
            .Color = {0.7f, 0.5f, 0.7f},
            .Normal = {0.0f, 1.0f, 0.0f},
            .Uv = {0.0f, 1.0f}
        },
        {
            .Position = {-1.0f, 0.25f, 0.75f},
            .Color = {0.7f, 0.5f, 0.7f},
            .Normal = {0.0f, 1.0f, 0.0f},
            .Uv = {0.0f, 0.0f}
        },
        {
            .Position = {1.0f, 0.25f, 0.75f},
            .Color = {0.7f, 0.5f, 0.7f},
            .Normal = {0.0f, 1.0f, 0.0f},
            .Uv = {1.0f, 0.0f}
        },
        {
            .Position = {1.0f, 0.25f, -0.75f},
            .Color = {0.7f, 0.5f, 0.7f},
            .Normal = {0.0f, 1.0f, 0.0f},
            .Uv = {1.0f, 1.0f}
        },

        //Bottom Face
        {
            .Position = {1.0f, -0.25f, 0.75f},
            .Color = {0.7f, 0.5f, 0.7f},
            .Normal = {0.0f, -1.0f, 0.0f},
            .Uv = {1.0f, 0.0f}
        },
        {
            .Position = {1.0f, -0.25f, -0.75f},
            .Color = {0.7f, 0.5f, 0.7f},
            .Normal = {0.0f, -1.0f, 0.0f},
            .Uv = {1.0f, 1.0f}
        },
        {
            .Position = {-1.0f, -0.25f, -0.75f},
            .Color = {0.7f, 0.5f, 0.7f},
            .Normal = {0.0f, -1.0f, 0.0f},
            .Uv = {0.0f, 1.0f}
        },
        {
            .Position = {-1.0f, -0.25f, 0.75f},
            .Color = {0.7f, 0.5f, 0.7f},
            .Normal = {0.0f, -1.0f, 0.0f},
            .Uv = {0.0f, 0.0f}
        }
        
    };



    static inline std::vector<uint32_t> cubeElements{
        0, 1, 2, 0, 2, 3, // Front face
        4, 5, 6, 4, 6, 7,  // Right face
        8, 9, 10, 8, 10, 11, // Back face
        12, 13, 14, 12, 14, 15, // Left face
        16, 17, 18, 16, 18, 19, // Top face
        20, 21, 22, 20, 22, 23 // Bottom face
    };




};
