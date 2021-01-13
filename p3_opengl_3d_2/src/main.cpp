
//
//  main.cpp
//  OpenGL_3D_1
//
//  Created by Alp Cihan on 11.12.2020.
//  Copyright © 2020 Alp Cihan. All rights reserved.
//

#include <iostream>
#include <vector>

#define GLM_FORCE_LEFT_HANDED
#include "glm/glm.hpp"
#include "glm/common.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/vector_angle.hpp"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "vector"

#include "opengl_utilities.h"
#include "mesh_generation.h"

/* Keep the global state inside this struct */
static struct {
    
    glm::dvec3 view_position = glm::dvec3(0,0,-4);
    
    const float alpha = 0.005; // move speed of the player
    const float rotate_s = 0.008; // rotation speed of the player
    
    float rot = 0.0; // store player rotation
    float fix = 0;
    
    const float radius = 5; // radius of the mars
    
    // Camera infos
    glm::vec3 cam_pos;
    glm::vec3 cam_lookat;
    glm::vec3 cam_up;
    glm::vec3 cam_right;
    bool free = false; // true = 3rd person camera, false = free movement
    
    // Player infos
    glm::vec3 player_position = glm::dvec3(0,0,-radius);// euc position
    glm::vec3 player_look_at = glm::vec3(0,1,0);
    glm::vec3 player_right = glm::vec3(1,0,0);
    glm::vec3 player_up = glm::vec3(1,0,-1);
    bool stop = false; // if collides, stop the player
    
    // Enemy infos
    std::vector<glm::vec3> enemy_positions = {glm::dvec3(1,1,1), glm::dvec3(4,1,-1.5)};
    std::vector<bool> enemy_stops = {false, false};
    
    glm::dvec2 mouse_position;
    glm::ivec2 screen_dimensions = glm::ivec2(960, 960);
    int rotation_speed = 50;
    
    GLint u_mouse_location;
    GLint u_model_location;
    GLint u_projection_view_location;
    
    GLint time;
    GLint shininess;
    
    bool key_hold = false;
    char key;
    
    std::vector <GLuint> textures; // [0] mars, [1] wheel, [2] body
    bool start = false;
    
} Globals;

/* GLFW Callback functions */
static void ErrorCallback(int error, const char* description)
{
    std::cerr << "Error: " << description << std::endl;
}

static void CursorPositionCallback(GLFWwindow* window, double x, double y)
{
    Globals.mouse_position.x = x;
    Globals.mouse_position.y = y;
}

static void WindowSizeCallback(GLFWwindow* window, int width, int height)
{
    Globals.screen_dimensions.x = width;
    Globals.screen_dimensions.y = height;
    
    glViewport(0, 0, width, height);
}

void MoveOnCircle(float a)
{
    float r = Globals.radius;
    
    glm::vec3 up = glm::normalize(Globals.player_position);
    glm::vec3 look = Globals.player_look_at;
    glm::vec3 right = glm::normalize(glm::cross(up,look)); // TODO: maybe swap the order
    
    // right.x*(x - px) + right.y*(y - py) + right.z*(z - pz) = 0
    
    glm::vec3 u = glm::normalize(up);
    glm::vec3 v = glm::normalize(look);
    
    float x = r*cos(a)*u.x + r*sin(a)*v.x;
    float y = r*cos(a)*u.y + r*sin(a)*v.y;
    float z = r*cos(a)*u.z + r*sin(a)*v.z;
    
    Globals.player_position = glm::vec3(x,y,z);
    
    up = glm::normalize(Globals.player_position);
    Globals.player_look_at = glm::normalize(glm::cross(right,up));
    Globals.player_right = glm::normalize(right);
    Globals.player_up = up;
}

// Key Controls
void key_calls(GLFWwindow* window)
{
    bool move = true;
    
    if(Globals.stop)
        return;
    
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        Globals.free = true;
    else if(glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
        Globals.free = false;
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        if(!Globals.free)
        {
            MoveOnCircle(Globals.alpha);
            Globals.fix += Globals.alpha;
            move = true;
            Globals.start = true;
        }
        else
            Globals.cam_pos += 0.05f*glm::normalize(Globals.player_look_at);
    }
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        if(!Globals.free)
        {
            MoveOnCircle(-Globals.alpha);
            Globals.fix -= Globals.alpha;
            move = true;
            Globals.start = true;
        }
        else
        {
            Globals.cam_pos -= 0.05f*glm::normalize(Globals.player_look_at);
        }
    }
    if(move && glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        if(!Globals.free)
        {
            Globals.player_look_at = glm::rotate(Globals.player_look_at, Globals.rotate_s, Globals.player_position);
            Globals.player_right = glm::cross(Globals.player_up,Globals.player_look_at);
            
            Globals.rot += Globals.rotate_s;
        }
        else
        {
            Globals.cam_pos += 0.05f*glm::normalize(Globals.player_right);
        }
        
    }
    
    if(move && glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        if(!Globals.free)
        {
            Globals.player_look_at = glm::rotate(Globals.player_look_at, -Globals.rotate_s, Globals.player_position);
            Globals.player_right = glm::cross(Globals.player_up,Globals.player_look_at);
            
            Globals.rot -= Globals.rotate_s;
        }
        else
            Globals.cam_pos -= 0.05f*glm::normalize(Globals.player_right);
    }
    if(Globals.free &&  glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
        Globals.cam_pos += 0.05f*glm::normalize(Globals.player_up);
    }
    if(Globals.free &&  glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
    {
        Globals.cam_pos -= 0.05f*glm::normalize(Globals.player_up);
    }
}
/* Change Scene Shaders */
void load_shader()
{
    char* texture_files[] = {
        "Assets/mars_1k_color.jpg",
        "Assets/wheel.jpg",
        "Assets/body.jpg"};
    
    stbi_set_flip_vertically_on_load(true);
    
    for(int i = 0; i < 3; i++)
    {
        auto filename = texture_files[i];
        
        int x,y,n;
        unsigned char *texture_data = stbi_load(filename, &x, &y, &n, 0);
        
        if (texture_data == NULL)
        {
            std::cout << "Texture " << filename << " failed to load." << std::endl;
            std::cout << "Error: " << stbi_failure_reason() << std::endl;
        }
        else
        {
            std::cout << "Texture " << filename << " is loadad, X: " << x << " Y: " << y << " N: " << n << std::endl;
        }
        
        GLuint texture;
        glGenTextures(1, &texture);
        
        
        if ((x * n) % 4 != 0)
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        
        
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
                     GL_TEXTURE_2D,
                     0, GL_RGBA,
                     x, y, 0, n == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, texture_data
                     );
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        glGenerateMipmap(GL_TEXTURE_2D);
        
        
        if ((x * n) % 4 != 0)
            glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
        // Free the texture
        stbi_image_free(texture_data);
        
        Globals.textures.push_back(texture);
    }
    
    GLuint shader_program = CreateProgramFromSources
    (
     R"VERTEX(
     #version 330 core
     
     layout(location = 0) in vec3 a_position;
     layout(location = 1) in vec3 a_normal;
     layout(location = 2) in vec2 a_uv;
     
     uniform mat4 u_model;
     uniform mat4 u_projection_view;
     
     out vec4 world_space_position;
     out vec3 world_space_normal;
     out vec2 vertex_uv;
     
     void main()
     {
     
     world_space_position = u_model * vec4(a_position, 1);
     world_space_normal = vec3(u_model * vec4(a_normal, 0));
     vertex_uv = a_uv;
     
     gl_Position = u_projection_view * world_space_position;
     
     }
     )VERTEX",
     
     R"FRAGMENT(
     #version 330 core
     
     uniform vec2 u_mouse_position;
     uniform sampler2D u_texture;
     
     in vec4 world_space_position;
     in vec3 world_space_normal;
     in vec2 vertex_uv;
     
     out vec4 out_color;
     
     void main()
     {
     vec3 color = vec3(0);
     
     vec3 surface_position = world_space_position.xyz;
     vec3 surface_normal = normalize(world_space_normal);
     vec2 surface_uv = vertex_uv;
     
     vec3 surface_color = texture(u_texture, vec2(vertex_uv)).rgb;
     
     float ambient = 1;
     vec3 ambient_color = vec3(0.7,0.7,0.7);
     color += ambient * ambient_color * surface_color;
     
     // Non Moving Point
     
     vec3 light_position = vec3(1,1,-1);
          
     vec3 light_direction = normalize(vec3(-1,-1, 1));
     vec3 to_light = -light_direction;
     vec3 light_color =  vec3(0.4,0.4,0.4);
     
     float diffuse_k = 1;
     float diffuse_intensity = max(0, dot(to_light, surface_normal));
     color += diffuse_k * diffuse_intensity * light_color * surface_color;
     
     vec3 view_dir = vec3(0, 0, -1);    //    Because we are using an orthograpic projection, and because of the direction of the projection
     vec3 halfway_dir = normalize(view_dir + to_light);
     float specular_k = 1;
     float shininess = 64;
     float specular_intensity = max(0, dot(halfway_dir, surface_normal));
     color += specular_k * pow(specular_intensity, shininess) * light_color;
     
     
     out_color = vec4(color,1);
     }
     )FRAGMENT"
     );
    
    if (shader_program == NULL)
    {
        glfwTerminate();
    }
    
    Globals.u_mouse_location = glGetUniformLocation(shader_program, "u_mouse_position");
    Globals.u_model_location = glGetUniformLocation(shader_program, "u_model");
    Globals.u_projection_view_location = glGetUniformLocation(shader_program, "u_projection_view");
    Globals.time = glGetUniformLocation(shader_program, "time");
    Globals.shininess = glGetUniformLocation(shader_program, "shininess");
    
    
    /* Texture */
    auto texture_location = glGetUniformLocation(shader_program, "u_Texture");
    glUniform1i(texture_location, 0); // 0th unit
    
    glActiveTexture(GL_TEXTURE0); // active 0th unit
    
    glUseProgram(shader_program); // use shader
}

float max(float a, float b)
{
    return a >= b ? a : b;
}
float min(float a, float b)
{
    return a <= b ? a : b;
}

// AABB Collusion check
void CollusionCheck()
{
    float r = 1.275;
    
    // Player
    glm::vec3 vec = Globals.player_position + r*Globals.player_look_at;
    glm::vec3 vec2 = Globals.player_position - r*Globals.player_look_at;
    float a_max_z = max(vec.z,vec2.z);
    float a_min_z = min(vec.z,vec2.z);
    
    vec = Globals.player_position + r*Globals.player_right;
    vec2 = Globals.player_position - r*Globals.player_right;
    float a_max_x = max(vec.x,vec2.x);
    float a_min_x = min(vec.x,vec2.x);
    
    vec = Globals.player_position + r*Globals.player_up;
    vec2 = Globals.player_position - r*Globals.player_up;
    float a_max_y = max(vec.y,vec2.y);
    float a_min_y = min(vec.y,vec2.y);
    
    for(int i = 0; i < Globals.enemy_positions.size(); i++)
    {
        vec = Globals.enemy_positions[i] + r*Globals.player_look_at;
        vec2 = Globals.enemy_positions[i] - r*Globals.player_look_at;
        float b_max_z = max(vec.z,vec2.z);
        float b_min_z = min(vec.z,vec2.z);
        
        vec = Globals.enemy_positions[i] + r*Globals.player_right;
        vec2 = Globals.enemy_positions[i] - r*Globals.player_right;
        float b_max_x = max(vec.x,vec2.x);
        float b_min_x = min(vec.x,vec2.x);
        
        vec = Globals.enemy_positions[i] + r*glm::normalize(Globals.enemy_positions[i]);
        vec2 = Globals.enemy_positions[i] - r*glm::normalize(Globals.enemy_positions[i]);
        float b_max_y = max(vec.y,vec2.y);
        float b_min_y = min(vec.y,vec2.y);
        
        /*if((a_min_x <= b_max_x && a_max_x >= b_min_x) &&
        (a_min_z <= b_max_z && a_max_z >= b_min_z) &&
           (a_min_y <= b_max_y && a_max_y >= b_min_y))
        {
            Globals.stop = true;
            Globals.enemy_stops[i] = true;
        }*/
        
        if(glm::length(Globals.enemy_positions[i] - Globals.player_position) < r)
        {
            Globals.stop = true;
            Globals.enemy_stops[i] = true;
        }
    }
}

void Rover(const VAO & sphereVAO, const VAO & donutVAO, const glm::dvec2 & mouse_position)
{
    glm::mat4 transform(1);
    
    /*std::cout <<"Player:" << Globals.player_position.x << " "  << Globals.player_position.y << " " << Globals.player_position.z << " " << std::endl;
    std::cout <<"Up:" << Globals.player_up.x << " "  << Globals.player_up.y << " " << Globals.player_up.z << " " << std::endl;
    std::cout <<"Look:" << Globals.player_look_at.x << " "  << Globals.player_look_at.y << " " << Globals.player_look_at.z << " " << std::endl;
    std::cout <<"Right:" << Globals.player_right.x << " "  << Globals.player_right.y << " " << Globals.player_right.z << " " << std::endl << std::endl;*/
    
    // Body

    transform = glm::translate(transform, Globals.player_position*1.05f);
    
    //transform = glm::rotate(transform, Globals.fix, Globals.player_right);

    transform = glm::scale(transform, glm::vec3(0.2));
    
    glBindTexture(GL_TEXTURE_2D, Globals.textures[2]);
    
    glBindVertexArray(sphereVAO.id);
    
    glUniformMatrix4fv(Globals.u_model_location, 1, GL_FALSE, glm::value_ptr(transform));
    
    glDrawElements(GL_TRIANGLES, sphereVAO.element_array_count, GL_UNSIGNED_INT, NULL);
    
    
    // Wheel 1
    
    transform = glm::mat4(1);

    transform = glm::translate(transform, Globals.player_position*1.05f + 0.5f*glm::rotate(Globals.player_right,45.f,Globals.player_up));

    transform = glm::rotate(transform, glm::degrees(90.f), Globals.player_position);
    transform = glm::rotate(transform, Globals.fix, Globals.player_right);

    
    transform = glm::scale(transform, glm::vec3(0.2));
    
    glBindTexture(GL_TEXTURE_2D, Globals.textures[1]);
    glBindVertexArray(donutVAO.id);
    glUniformMatrix4fv(Globals.u_model_location, 1, GL_FALSE, glm::value_ptr(transform));
    glDrawElements(GL_TRIANGLES, donutVAO.element_array_count, GL_UNSIGNED_INT, NULL);
    
    // Wheel 2
    transform = glm::mat4(1);
    
    transform = glm::translate(transform, Globals.player_position*1.05f + 0.5f*glm::rotate(-Globals.player_right,45.f,Globals.player_up));
    transform = glm::rotate(transform, glm::degrees(90.f), Globals.player_position);
    transform = glm::rotate(transform, Globals.fix, Globals.player_right);
    
    transform = glm::scale(transform, glm::vec3(0.2));
    
    glBindTexture(GL_TEXTURE_2D, Globals.textures[1]);
    glBindVertexArray(donutVAO.id);
    glUniformMatrix4fv(Globals.u_model_location, 1, GL_FALSE, glm::value_ptr(transform));
    glDrawElements(GL_TRIANGLES, donutVAO.element_array_count, GL_UNSIGNED_INT, NULL);
    
    // Wheel 3
    transform = glm::mat4(1);
    
    transform = glm::translate(transform, Globals.player_position*1.05f + 0.5f*glm::rotate(-Globals.player_right,45.f,-Globals.player_up));
    transform = glm::rotate(transform, glm::degrees(90.f), Globals.player_position);
    transform = glm::rotate(transform, Globals.fix, Globals.player_right);
    
    transform = glm::scale(transform, glm::vec3(0.2));
    
    glBindTexture(GL_TEXTURE_2D, Globals.textures[1]);
    glBindVertexArray(donutVAO.id);
    glUniformMatrix4fv(Globals.u_model_location, 1, GL_FALSE, glm::value_ptr(transform));
    glDrawElements(GL_TRIANGLES, donutVAO.element_array_count, GL_UNSIGNED_INT, NULL);
    
    // Wheel 3
       transform = glm::mat4(1);
       
       transform = glm::translate(transform, Globals.player_position*1.05f + 0.5f*glm::rotate(Globals.player_right,45.f,-Globals.player_up));
       transform = glm::rotate(transform, glm::degrees(90.f), Globals.player_position);
       transform = glm::rotate(transform, Globals.fix, Globals.player_right);
       
       transform = glm::scale(transform, glm::vec3(0.2));
       
       glBindTexture(GL_TEXTURE_2D, Globals.textures[1]);
       glBindVertexArray(donutVAO.id);
       glUniformMatrix4fv(Globals.u_model_location, 1, GL_FALSE, glm::value_ptr(transform));
       glDrawElements(GL_TRIANGLES, donutVAO.element_array_count, GL_UNSIGNED_INT, NULL);
     
}

void Enemy(const VAO & sphereVAO, const VAO & donutVAO, const int i)
{

    // Chase the player on spherical surface
    if(!Globals.enemy_stops[i])
    {
        float distance = glm::length(Globals.player_position-Globals.enemy_positions[i]) / 1000.0;
        if (distance != 0)
            Globals.enemy_positions[i] = glm::mix(Globals.player_position, Globals.enemy_positions[i], 0.988f + distance );
            
        // Map the enemy on the spherical surface
        float len = glm::length(Globals.enemy_positions[i]);
        float off = Globals.radius - len;
        glm::vec3 unit = glm::normalize(Globals.enemy_positions[i]);
        Globals.enemy_positions[i] += unit * (off + 0.2f) ;
        
        if(i == 0 && glm::length(Globals.enemy_positions[0]-Globals.enemy_positions[1]) < 0.5)
            Globals.enemy_positions[0] +=  0.01f*glm::normalize(Globals.enemy_positions[0]-Globals.enemy_positions[1]);
    }

    glm::mat4 transform(1);
    
    transform = glm::translate(transform, Globals.enemy_positions[i]);
    //transform = glm::rotate(transform, float(Globals.rot), glm::vec3(0,0,1));
    transform = glm::scale(transform, glm::vec3(0.2));
    
    glBindTexture(GL_TEXTURE_2D, Globals.textures[2]);
    
    glBindVertexArray(sphereVAO.id);
    glUniformMatrix4fv(Globals.u_model_location, 1, GL_FALSE, glm::value_ptr(transform));
    glDrawElements(GL_TRIANGLES, donutVAO.element_array_count, GL_UNSIGNED_INT, NULL);
    
    
    // Wheel 1
    
    transform = glm::mat4(1);

    transform = glm::translate(transform, Globals.enemy_positions[i]*1.05f + 0.4f*glm::rotate(Globals.player_right,45.f,Globals.enemy_positions[i]));
    transform = glm::rotate(transform, glm::degrees(90.f), Globals.enemy_positions[i]);
    transform = glm::rotate(transform, Globals.fix, Globals.player_right);
    
    transform = glm::scale(transform, glm::vec3(0.2));
    
    glBindTexture(GL_TEXTURE_2D, Globals.textures[1]);
    glBindVertexArray(donutVAO.id);
    glUniformMatrix4fv(Globals.u_model_location, 1, GL_FALSE, glm::value_ptr(transform));
    glDrawElements(GL_TRIANGLES, donutVAO.element_array_count, GL_UNSIGNED_INT, NULL);
    
    // Wheel 2
    transform = glm::mat4(1);
    
    transform = glm::translate(transform, Globals.enemy_positions[i]*1.05f + 0.4f*glm::rotate(-Globals.player_right,45.f,Globals.enemy_positions[i]));
    transform = glm::rotate(transform, glm::degrees(90.f), Globals.enemy_positions[i]);
    transform = glm::rotate(transform, Globals.fix, Globals.player_right);
    
    transform = glm::scale(transform, glm::vec3(0.2));
    
    glBindTexture(GL_TEXTURE_2D, Globals.textures[1]);
    glBindVertexArray(donutVAO.id);
    glUniformMatrix4fv(Globals.u_model_location, 1, GL_FALSE, glm::value_ptr(transform));
    glDrawElements(GL_TRIANGLES, donutVAO.element_array_count, GL_UNSIGNED_INT, NULL);
    
    // Wheel 3
    transform = glm::mat4(1);
    
    transform = glm::translate(transform, Globals.enemy_positions[i]*1.05f + 0.4f*glm::rotate(-Globals.player_right,45.f,-Globals.enemy_positions[i]));
    transform = glm::rotate(transform, glm::degrees(90.f), Globals.enemy_positions[i]);
    transform = glm::rotate(transform, Globals.fix, Globals.player_right);
    
    transform = glm::scale(transform, glm::vec3(0.2));
    
    glBindTexture(GL_TEXTURE_2D, Globals.textures[1]);
    glBindVertexArray(donutVAO.id);
    glUniformMatrix4fv(Globals.u_model_location, 1, GL_FALSE, glm::value_ptr(transform));
    glDrawElements(GL_TRIANGLES, donutVAO.element_array_count, GL_UNSIGNED_INT, NULL);
    
    // Wheel 3
       transform = glm::mat4(1);
       
       transform = glm::translate(transform, Globals.enemy_positions[i]*1.05f + 0.4f*glm::rotate(Globals.player_right,45.f,-Globals.enemy_positions[i]));
       transform = glm::rotate(transform, glm::degrees(90.f), Globals.enemy_positions[i]);
       transform = glm::rotate(transform, Globals.fix, Globals.player_right);
       
       transform = glm::scale(transform, glm::vec3(0.2));
       
       glBindTexture(GL_TEXTURE_2D, Globals.textures[1]);
       glBindVertexArray(donutVAO.id);
       glUniformMatrix4fv(Globals.u_model_location, 1, GL_FALSE, glm::value_ptr(transform));
       glDrawElements(GL_TRIANGLES, donutVAO.element_array_count, GL_UNSIGNED_INT, NULL);
}

void Mars(const VAO & shapeVAO)
{
    glm::mat4 transform(1);
    
    transform = glm::translate(transform, glm::vec3(0, 0, 0));
    transform = glm::scale(transform, glm::vec3(Globals.radius));
    
    glBindVertexArray(shapeVAO.id);
    
    glBindTexture(GL_TEXTURE_2D, Globals.textures[0]);
    
    glUniformMatrix4fv(Globals.u_model_location, 1, GL_FALSE, glm::value_ptr(transform));
    
    glDrawElements(GL_TRIANGLES, shapeVAO.element_array_count, GL_UNSIGNED_INT, NULL);
}

void Scene(const VAO & sphereVAO,const VAO & donutVAO, const glm::dvec2 & mouse_position, glm::dvec3 & chasing_pos)
{
    // Camera Control
    glm::vec3 lookAt;
    glm::vec3 up;
    glm::vec3 position;
    glm::vec3 right;
    
    // 3rd person movement
    if(!Globals.free)
    {
        lookAt= glm::rotate(Globals.player_look_at, 2.f*float(mouse_position.x), Globals.player_up);
        lookAt = 5.0f*glm::rotate(lookAt, -0.8f*float(mouse_position.y), Globals.player_right);
        position= 10.0f*glm::normalize((-Globals.player_look_at + 2.0f*Globals.player_up));
        up = Globals.player_up;
        right = Globals.player_right;
    }
    // Independent movement
    else
    {
        lookAt= glm::rotate(Globals.cam_lookat, 2.f*float(mouse_position.x), Globals.player_up);
        lookAt = 5.0f*glm::rotate(lookAt, -0.8f*float(mouse_position.y), Globals.player_right);
        up = Globals.player_up;
        right = Globals.player_right;
    }
    
    // Camera
    auto view = glm::lookAt(
                            glm::vec3(Globals.cam_pos), // view position
                            glm::vec3(lookAt), // view lookat
                            glm::vec3(up)
                            );
    
    auto projection = glm::perspective(glm::radians(45.f), 1.f, 0.1f, 10.f); // Space borders
    glUniformMatrix4fv(Globals.u_projection_view_location, 1, GL_FALSE, glm::value_ptr(projection * view));
    
    // Objects
    Mars(sphereVAO);
    
    Rover(sphereVAO, donutVAO, mouse_position);
    
    if(Globals.start == true)
    {
        Enemy(sphereVAO, donutVAO, 0);
        
        Enemy(sphereVAO, donutVAO, 1);
    }
    
    // AABB Collusion Check
    CollusionCheck();
    
    if(!Globals.free)
    {
        Globals.cam_pos = position;
        Globals.cam_lookat = Globals.player_look_at;
        Globals.cam_up = up;
        Globals.player_right = right;
    }
}


int main(int argc, char* argv[])
{
    /* Set GLFW error callback */
    glfwSetErrorCallback(ErrorCallback);
    
    /* Initialize the library */
    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    
    /* Create a windowed mode window and its OpenGL context */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow
    ( Globals.screen_dimensions.x, Globals.screen_dimensions.y,
     "Alp Cihan", NULL, NULL
     );
    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    /* Move window to a certain position [do not change] */
    glfwSetWindowPos(window, 10, 50);
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    /* Enable VSync */
    glfwSwapInterval(1);
    
    /* Load OpenGL extensions with GLAD */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    /* Set GLFW Callbacks */
    glfwSetCursorPosCallback(window, CursorPositionCallback);
    glfwSetWindowSizeCallback(window, WindowSizeCallback);
    
    
    /* Configure OpenGL */
    glClearColor(0, 0, 0, 0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    /* Creating Meshes */
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
    std::vector<GLuint> indices;
    
    // Sphere
    GenerateParametricShapeFrom2D(positions, normals, uvs, indices, ParametricHalfCircle, 64, 64);
    VAO sphereVAO(positions, normals, uvs, indices);
    {positions.clear();normals.clear();indices.clear();} // clear the attributes
    
    GenerateParametricShapeFrom2D(positions, normals, uvs, indices, ParametricCircle, 64, 64);
    VAO donutVAO(positions, normals, uvs, indices);
    {positions.clear();normals.clear();indices.clear();} // clear the attributes
    
    
    
    // Shader Temp
    load_shader();
    
    glm::dvec3 chase_position = glm::dvec3(0,0,0);
    MoveOnCircle(Globals.alpha);
    Globals.fix += Globals.alpha;
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        // Clear
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Change the position of a vertex dynamically
        auto mouse_position = Globals.mouse_position / glm::dvec2(Globals.screen_dimensions);
        mouse_position.y = 1. - mouse_position.y;
        mouse_position = mouse_position * 2. - 1.;
        
        // Light
        glUniform2fv(Globals.u_mouse_location, 1, glm::value_ptr(glm::vec2(mouse_position))); // light position
        
        // Player Control
        key_calls(window);
        
        // Scene
        Scene(sphereVAO, donutVAO, mouse_position, chase_position);
        
        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        
        /* Poll for and process events */
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}
