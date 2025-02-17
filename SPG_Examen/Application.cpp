#include <iostream>
#include <windows.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <GL/glu.h>
#include <stb/stb_image.h>
#include <vector>
#include <cstdlib>
#include <map>
#include <GL/gl.h>

#define M_PI 3.14159265358979323846

float targetCamX = 0.0f, targetCamY = 0.0f, targetCamZ = 4.0f;
bool moveCamera = false;
double lastTime = 0.0;

GLfloat lightDirection[] = { 0.0f, 1.0f, 0.0f, 0.0f };

struct Ball {
    float x, y, z;
    float vx, vy, vz;
    float radius;
    float r, g, b;
};

void drawBall(float radius, int stacks, int slices) {
    float phi, theta;
    float x, y, z;
    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i <= stacks; ++i) {
        theta = i * M_PI / stacks;
        for (int j = 0; j <= slices; ++j) {
            phi = j * 2 * M_PI / slices;
            x = radius * cos(phi) * sin(theta);
            y = radius * sin(phi) * sin(theta);
            z = radius * cos(theta);
            glVertex3f(x, y, z);
            x = radius * cos(phi) * sin(theta + M_PI / stacks);
            y = radius * sin(phi) * sin(theta + M_PI / stacks);
            z = radius * cos(theta + M_PI / stacks);
            glVertex3f(x, y, z);
        }
    }
    glEnd();
}

void drawBuildings(float x, float y, float z, float size, GLuint textureID) {
    float halfSize = size / 2.0f;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glBegin(GL_QUADS);
    glNormal3f(1.0f, 1.0f, 1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x - halfSize, y - halfSize, z + halfSize);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x + halfSize, y - halfSize, z + halfSize);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x + halfSize, y + halfSize + 2.0f, z + halfSize);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x - halfSize, y + halfSize + 2.0f, z + halfSize);


    glTexCoord2f(0.0f, 0.0f); glVertex3f(x - halfSize, y - halfSize, z - halfSize);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x + halfSize, y - halfSize, z - halfSize);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x + halfSize, y + halfSize + 2.0f, z - halfSize);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x - halfSize, y + halfSize + 2.0f, z - halfSize);

    glTexCoord2f(0.0f, 0.0f); glVertex3f(x - halfSize, y + halfSize + 2.0f, z - halfSize);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x + halfSize, y + halfSize + 2.0f, z - halfSize);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x + halfSize, y + halfSize + 2.0f, z + halfSize);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x - halfSize, y + halfSize + 2.0f, z + halfSize);

    glTexCoord2f(0.0f, 0.0f); glVertex3f(x - halfSize, y - halfSize, z - halfSize);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x + halfSize, y - halfSize, z - halfSize);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x + halfSize, y - halfSize, z + halfSize);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x - halfSize, y - halfSize, z + halfSize);

    glTexCoord2f(0.0f, 1.0f); glVertex3f(x + halfSize, y - halfSize, z - halfSize);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x + halfSize, y + halfSize + 2.0f, z - halfSize);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x + halfSize, y + halfSize + 2.0f, z + halfSize);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x + halfSize, y - halfSize, z + halfSize);

    glTexCoord2f(0.0f, 0.0f); glVertex3f(x - halfSize, y - halfSize, z - halfSize);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x - halfSize, y + halfSize + 2.0f, z - halfSize);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x - halfSize, y + halfSize + 2.0f, z + halfSize);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x - halfSize, y - halfSize, z + halfSize);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
}

std::map<std::pair<float, float>, std::pair<float, GLuint>> buildingDataMap;

void positionBuildings(const std::vector<GLuint>& textures) {
    const float minSize = 2.5f;
    const float maxSize = 3.2f;

    for (float cx = -9.6f; cx <= 9.6f; cx += 6.4f) {
        for (float cz = -9.6f; cz <= 9.6f; cz += 6.4f) {
            auto pos = std::make_pair(cx, cz);
            if (buildingDataMap.find(pos) == buildingDataMap.end()) {
                float cubeSize = minSize + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maxSize - minSize)));

                GLuint selectedTextureID = textures[rand() % textures.size()];
                buildingDataMap[pos] = std::make_pair(cubeSize, selectedTextureID);
            }

            float cubeSize = buildingDataMap[pos].first;
            GLuint textureID = buildingDataMap[pos].second;

            const float bottomY = -1.0f + cubeSize / 2.0f;
            float adjustedY = bottomY;

            drawBuildings(cx, adjustedY, cz, cubeSize, textureID);
        }
    }
}


void drawTrees(float x, float y, float z, float size, GLuint textureID) {
    float halfSize = size / 2.0f;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glBegin(GL_QUADS);
    glNormal3f(1.0f, 1.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x - halfSize, y + size - 1.8f, z + halfSize);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x + halfSize, y + size - 1.8f, z - halfSize);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x + halfSize, y - 1.8f, z - halfSize);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x - halfSize, y - 1.8f, z + halfSize);

    glTexCoord2f(0.0f, 0.0f); glVertex3f(x - halfSize, y + size - 1.8f, z - halfSize);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x + halfSize, y + size - 1.8f, z + halfSize);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x + halfSize, y - 1.8f, z + halfSize);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x - halfSize, y - 1.8f, z - halfSize);
    glEnd();

    glDisable(GL_TEXTURE_2D); 
    glEnable(GL_DEPTH_TEST);
}

std::map<std::pair<float, float>, GLuint> buildingTextureMap;

void positionTrees(const std::vector<GLuint>& textures) {
    const float treeSize = 3.2f;
    const float bottomY = -1.0f + treeSize / 2.0f;

    for (float cx = -4.8f; cx <= 4.8f; cx += 3.2f) {
        for (float cz = -4.8f; cz <= 4.8f; cz += 3.2f) {
            float adjustedY = bottomY;
            auto pos = std::make_pair(cx, cz);
            if (buildingTextureMap.find(pos) == buildingTextureMap.end()) {
                GLuint selectedTextureID = textures[rand() % textures.size()];
                buildingTextureMap[pos] = selectedTextureID;
            }

            GLuint textureID = buildingTextureMap[pos];
            drawTrees(cx, adjustedY, cz, treeSize, textureID);
        }
    }
}

bool checkCollisionWithBuilding(float x, float y, float z, float radius) {
    const float cubeSize = 3.2f;
    const float bottomY = -1.0f + cubeSize / 2.0f;

    for (float cx = -9.6f; cx <= 9.6f; cx += 6.4f) {
        for (float cz = -9.6f; cz <= 9.6f; cz += 6.4f) {
            float dx = x - cx;
            float dz = z - cz;

            float distance = sqrt(dx * dx + dz * dz);

            if (distance < radius + cubeSize / 2.0f && y - radius <= bottomY + cubeSize / 2.0f) {
                return true;
            }
        }
    }

    return false;
}

bool checkCollision(float x, float y, float z) {

    if (y <= -0.5f) {
        return true;
    }

    if (checkCollisionWithBuilding(x, y, z, 1)) {
        return true;
    }

    return false;
}

GLuint loadTexture(const char* filePath) {
    int widthImg, heightImg, numColCh;
    unsigned char* bytes = stbi_load(filePath, &widthImg, &heightImg, &numColCh, 4);

    if (!bytes) {
        std::cerr << "Failed to load texture: " << filePath << std::endl;
        return 0;
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthImg, heightImg, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(bytes);
    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}



int main(void)
{
    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    window = glfwCreateWindow(1024, 768, "City", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize Glad" << std::endl;
        return -1;
    }


    glEnable(GL_DEPTH_TEST);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glLightfv(GL_LIGHT0, GL_POSITION, lightDirection);
    GLfloat lightColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);

    GLfloat materialColor[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, materialColor);


    const int numBalls = 3;
    Ball balls[numBalls];

    balls[0].x = 0.5f; balls[0].y = 0.5f; balls[0].z = 0.5f;
    balls[0].vx = 0.001f; balls[0].vy = 0.002f; balls[0].vz = 0.001f;
    balls[0].radius = 0.15f; balls[0].r = 0.0f; balls[0].g = 1.0f; balls[0].b = 1.0f;

    balls[1].x = -0.5f; balls[1].y = -0.5f; balls[1].z = -0.5f;
    balls[1].vx = 0.002f; balls[1].vy = 0.001f; balls[1].vz = 0.0015f;
    balls[1].radius = 0.25f; balls[1].r = 1.0f; balls[1].g = 0.0f; balls[1].b = 0.0f;

    balls[2].x = 0.0f; balls[2].y = 0.0f; balls[2].z = 0.0f;
    balls[2].vx = 0.0015f; balls[2].vy = 0.001f; balls[2].vz = 0.002f;
    balls[2].radius = 0.05f; balls[2].r = 0.0f; balls[2].g = 0.0f; balls[2].b = 1.0f;



    GLuint GrassTexture = loadTexture("Resources/grass4.png");
    GLuint SkyTexture = loadTexture("Resources/sky.png");
    GLuint BuildingTexture1 = loadTexture("Resources/building.png");
    GLuint BuildingTexture2 = loadTexture("Resources/building2.png");
    GLuint BuildingTexture3 = loadTexture("Resources/building3.png");
    GLuint BuildingTexture4 = loadTexture("Resources/building4.png");
    GLuint BuildingTexture5 = loadTexture("Resources/building5.png");
    GLuint BuildingTexture6 = loadTexture("Resources/building6.png");
    GLuint TreeTexture1 = loadTexture("Resources/tree1.png");
    GLuint TreeTexture2 = loadTexture("Resources/tree2.png");
    GLuint TreeTexture3 = loadTexture("Resources/tree3.png");
    GLuint RoadTexture1 = loadTexture("Resources/road1.png");
    GLuint RoadTexture2 = loadTexture("Resources/road2.png");
    GLuint HorizonTexture = loadTexture("Resources/horizon.png");

    std::vector<GLuint> buildingTextures;
    std::vector<GLuint> treeTextures;

    buildingTextures.push_back(BuildingTexture1);
    buildingTextures.push_back(BuildingTexture2);
    buildingTextures.push_back(BuildingTexture3);
    buildingTextures.push_back(BuildingTexture4);
    buildingTextures.push_back(BuildingTexture5);
    buildingTextures.push_back(BuildingTexture6);

    treeTextures.push_back(TreeTexture1);
    treeTextures.push_back(TreeTexture2);
    treeTextures.push_back(TreeTexture3);

    while (!glfwWindowShouldClose(window))
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        double currentTime = glfwGetTime();
        double deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        for (int i = 0; i < numBalls; ++i) {
            balls[i].x += balls[i].vx;
            balls[i].y += balls[i].vy;
            balls[i].z += balls[i].vz;

            balls[i].vy -= 0.000005f;

            if (balls[i].x + balls[i].radius >= 16.0f) {
                balls[i].vx = -abs(balls[i].vx);
                balls[i].x = 16.0f - balls[i].radius;
            }
            else if (balls[i].x - balls[i].radius <= -16.0f) {
                balls[i].vx = abs(balls[i].vx);
                balls[i].x = -16.0f + balls[i].radius;
            }
            if (balls[i].y + balls[i].radius >= 16.0f) {
                balls[i].vy = -abs(balls[i].vy);
                balls[i].y = 16.0f - balls[i].radius;
            }
            else if (balls[i].y - balls[i].radius <= -1.0f) {
                balls[i].vy = abs(balls[i].vy);
                balls[i].y = -1.0f + balls[i].radius;
            }
            if (balls[i].z + balls[i].radius >= 16.0f) {
                balls[i].vz = -abs(balls[i].vz);
                balls[i].z = 16.0f - balls[i].radius;
            }
            else if (balls[i].z - balls[i].radius <= -16.0f) {
                balls[i].vz = abs(balls[i].vz);
                balls[i].z = -16.0f + balls[i].radius;
            }

            if (checkCollisionWithBuilding(balls[i].x, balls[i].y, balls[i].z, balls[i].radius)) {
                balls[i].vx = -balls[i].vx;
                balls[i].vz = -balls[i].vz;
            }
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0f, 1.0f, 0.1f, 100.0f);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        float newTargetCamX = targetCamX;
        float newTargetCamY = targetCamY;
        float newTargetCamZ = targetCamZ;

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            newTargetCamX -= 0.5f * deltaTime; // Left X
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            newTargetCamX += 0.5f * deltaTime; // Right X
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            newTargetCamZ -= 0.5f * deltaTime; // Forward Z
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            newTargetCamZ += 0.5f * deltaTime; // Backward Z
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            newTargetCamY += 0.5f * deltaTime; // Up Y
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        {
            newTargetCamY -= 0.5f * deltaTime; // Down Y
        }

        if (!checkCollision(newTargetCamX, newTargetCamY, newTargetCamZ)) {
            targetCamX = newTargetCamX;
            targetCamY = newTargetCamY;
            targetCamZ = newTargetCamZ;
        }

        gluLookAt(targetCamZ, targetCamY, targetCamX, 0.0f, 0.0f, 0.0f, 0.0f, 1000.0f, 1.0f);

        // Sky
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, SkyTexture);

        glBegin(GL_QUADS);
        glColor3f(1.0f, 1.0f, 1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-16.0f, 16.0f, -16.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-16.0f, 16.0f, 16.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(16.0f, 16.0f, 16.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(16.0f, 16.0f, -16.0f);

        glEnd();

        glDisable(GL_TEXTURE_2D);
        glEnable(GL_DEPTH_TEST);

        // Horizon
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, HorizonTexture);

        glBegin(GL_QUADS);
        glColor3f(1.0f, 1.0f, 1.0f);

        glTexCoord2f(0.0f, 1.0f); glVertex3f(-16.0f, -16.0f, 16.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-16.0f, 16.0f, 16.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(16.0f, 16.0f, 16.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(16.0f, -16.0f, 16.0f);

        glTexCoord2f(0.0f, 1.0f); glVertex3f(-16.0f, -16.0f, -16.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-16.0f, 16.0f, -16.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-16.0f, 16.0f, 16.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-16.0f, -16.0f, 16.0f);

        glTexCoord2f(0.0f, 1.0f); glVertex3f(16.0f, -16.0f, -16.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(16.0f, 16.0f, -16.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(16.0f, 16.0f, 16.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(16.0f, -16.0f, 16.0f);

        glTexCoord2f(0.0f, 1.0f); glVertex3f(-16.0f, -16.0f, -16.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-16.0f, 16.0f, -16.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(16.0f, 16.0f, -16.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(16.0f, -16.0f, -16.0f);

        glEnd();

        glDisable(GL_TEXTURE_2D);
        glEnable(GL_DEPTH_TEST);

        // Grass
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, GrassTexture);

        glBegin(GL_QUADS);
        glColor3f(1.0f, 1.0f, 1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-16.0f, -1.0f, -16.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-16.0f, -1.0f, 16.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(16.0f, -1.0f, 16.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(16.0f, -1.0f, -16.0f);
        glEnd();

        glDisable(GL_TEXTURE_2D);

        // Road1
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, RoadTexture1);

        glBegin(GL_QUADS);
        glColor3f(1.0f, 1.0f, 1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-16.0f, -0.998f, -1.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-16.0f, -0.998f, 1.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(16.0f, -0.998f, 1.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(16.0f, -0.998f, -1.0f);
        glEnd();

        glDisable(GL_TEXTURE_2D);


        // Road2
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, RoadTexture1);

        glBegin(GL_QUADS);
        glColor3f(1.0f, 1.0f, 1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -0.999f, -16.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, -0.999f, 16.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, -0.999f, 16.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -0.999f, -16.0f);
        glEnd();

        glDisable(GL_TEXTURE_2D);

        // Crossroad
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, RoadTexture2);

        glBegin(GL_QUADS);
        glColor3f(1.0f, 1.0f, 1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -0.997f, -1.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -0.997f, 1.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, -0.997f, 1.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0f, -0.997f, -1.0f);
        glEnd();

        glDisable(GL_TEXTURE_2D);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        positionBuildings(buildingTextures);
        glDisable(GL_BLEND);
        

        for (int i = 0; i < numBalls; ++i) {
            glPushMatrix();
            glTranslatef(balls[i].x, balls[i].y, balls[i].z);
            glColor3f(balls[i].r, balls[i].g, balls[i].b);
            drawBall(balls[i].radius, 20, 20);
            glPopMatrix();
        }
        glColor3f(1.0f, 1.0f, 1.0f);

        glEnable(GL_BLEND);
        positionTrees(treeTextures);
        glDisable(GL_BLEND);




        glDisable(GL_DEPTH_TEST);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteTextures(1, &GrassTexture);
    glDeleteTextures(1, &SkyTexture);
    glDeleteTextures(1, &BuildingTexture1);
    glDeleteTextures(1, &BuildingTexture2);
    glDeleteTextures(1, &BuildingTexture3);
    glDeleteTextures(1, &BuildingTexture4);
    glDeleteTextures(1, &BuildingTexture5);
    glDeleteTextures(1, &BuildingTexture6);
    glDeleteTextures(1, &TreeTexture1);
    glDeleteTextures(1, &TreeTexture2);
    glDeleteTextures(1, &TreeTexture3);
    glDeleteTextures(1, &RoadTexture1);
    glDeleteTextures(1, &RoadTexture2);
    glDeleteTextures(1, &HorizonTexture);
    glfwTerminate();
    return 0;
}
