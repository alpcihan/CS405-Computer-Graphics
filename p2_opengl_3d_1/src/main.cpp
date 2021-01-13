//
//  main.cpp
//  OpenGL_3D_1
//
//  Created by Alp Cihan on 11.12.2020.
//  Copyright © 2020 Alp Cihan. All rights reserved.
//

#include <iostream>
#include <vector>

#include "glm/glm.hpp"
#include "glm/common.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "opengl_utilities.h"
#include "mesh_generation.h"

/* Keep the global state inside this struct */
static struct {
    glm::dvec2 mouse_position;
    glm::ivec2 screen_dimensions = glm::ivec2(960, 960);
    int rotation_speed = 10;
    unsigned int scene = 1; // 1,2,3,4,5,6
    
    GLint u_mouse_location;
    GLint u_transform_location;
    GLint material_color;
    
    GLint time;
    GLint noise_k;
    GLint experiment_shader;
    GLint shininess;
    
    float noise_k_val = 0;
    const float noise_max = 0.05;
    const float noise_change_k = 0.002;
    unsigned short int noise_mode = 0; // 0 idle, 1 increase, 2 decrease
    
    int experiment = true;
    
    GLint start_time;
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

/* Change Scene Shaders */
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_Q && action == GLFW_PRESS)
        Globals.scene = 1;
    else if (key == GLFW_KEY_W && action == GLFW_PRESS)
        Globals.scene = 2;
    else if (key == GLFW_KEY_E && action == GLFW_PRESS)
        Globals.scene = 3;
    else if (key == GLFW_KEY_R && action == GLFW_PRESS)
        Globals.scene = 4;
    else if (key == GLFW_KEY_T && action == GLFW_PRESS)
        Globals.scene = 5;
    else if (key == GLFW_KEY_Y && action == GLFW_PRESS)
    {
        Globals.scene = 6;
        Globals.start_time = glfwGetTime();
    }
    else if(Globals.scene == 6 && key== GLFW_KEY_X && action == GLFW_PRESS && !Globals.experiment)
    {
        if(Globals.noise_mode == 1)
            Globals.noise_mode = 2;
        else
            Globals.noise_mode = 1;
        return;
    }
    else if(Globals.scene == 6 && key== GLFW_KEY_Z && action == GLFW_PRESS)
    {
        Globals.experiment = !Globals.experiment;
        if(Globals.experiment)
        {
            Globals.noise_mode = 0;
            Globals.noise_k_val = 0;
            Globals.start_time = glfwGetTime();
        }
    }
    
    
    GLuint shader_program = NULL;
    
    switch (Globals.scene) {
            
        case 1: // White color only
            shader_program = CreateProgramFromSources
            (
             R"VERTEX(
             #version 330 core
             
             layout(location = 0) in vec3 a_position;
             
             uniform mat4 u_transform;
             
             void main()
             {
             gl_Position = u_transform * vec4(a_position, 1);
             }
             )VERTEX",
             
             R"FRAGMENT(
             #version 330 core
             
             out vec4 out_color;
             
             void main()
             {
             out_color = vec4(1,1,1, 1); // only white
             }
             )FRAGMENT");
            break;
            
        case 2: // Color by normal vectors
            shader_program = CreateProgramFromSources
            (
             R"VERTEX(
             #version 330 core
             
             layout(location = 0) in vec3 a_position;
             layout(location = 1) in vec3 a_normal;
             
             uniform mat4 u_transform;
             
             out vec3 vertex_position;
             out vec3 vertex_normal;
             
             void main()
             {
             gl_Position = u_transform * vec4(a_position, 1);
             vertex_normal = vec3(u_transform * vec4(a_normal,0));
             vertex_position = gl_Position.xyz;
             }
             )VERTEX",
             
             R"FRAGMENT(
             #version 330 core
             
             uniform vec2 u_mouse_position;
             
             in vec3 vertex_position;
             in vec3 vertex_normal;
             
             out vec4 out_color;
             
             
             void main()
             {
             out_color = vec4(normalize(vertex_normal), 1); // color with light
             }
             )FRAGMENT"
             );
            break;
            
        case 3: // Blinn-Phong (gray only)
            shader_program = CreateProgramFromSources
            (
             R"VERTEX(
             #version 330 core
             
             layout(location = 0) in vec3 a_position;
             layout(location = 1) in vec3 a_normal;
             
             uniform mat4 u_transform;
             
             out vec3 vertex_position;
             out vec3 vertex_normal;
             
             void main()
             {
             gl_Position = u_transform * vec4(a_position, 1);
             vertex_normal = vec3(u_transform * vec4(a_normal, 0));
             vertex_position = vec3(gl_Position);
             }
             )VERTEX",
             
             R"FRAGMENT(
             #version 330 core
             
             uniform vec2 u_mouse_position;
             
             in vec3 vertex_position;
             in vec3 vertex_normal;
             
             out vec4 out_color;
             
             void main()
             {
             vec3 color = vec3(0);
             
             // Non Moving Directional Light
             vec3 surface_color = vec3(0.5, 0.5, 0.5);
             vec3 surface_position = vertex_position;
             vec3 surface_normal = normalize(vertex_normal);
             
             float ambient = 1;
             vec3 ambient_color = vec3(0.5,0.5,0.5);
             color += ambient * ambient_color * surface_color;
             
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
             
             out_color = vec4(color, 1);
             }
             )FRAGMENT"
             );
            break;
            
        case 4: // Blinn-Phong (colored + point light)
            shader_program = CreateProgramFromSources
            (
             R"VERTEX(
             #version 330 core
             
             layout(location = 0) in vec3 a_position;
             layout(location = 1) in vec3 a_normal;
             
             uniform mat4 u_transform;
             
             out vec3 vertex_position;
             out vec3 vertex_normal;
             
             void main()
             {
             gl_Position = u_transform * vec4(a_position, 1);
             vertex_normal = vec3(u_transform * vec4(a_normal, 0));
             vertex_position = vec3(gl_Position);
             }
             )VERTEX",
             
             R"FRAGMENT(
             #version 330 core
             
             uniform vec2 u_mouse_position;
             uniform vec3 material_color;
             uniform float shininess;
             in vec3 vertex_position;
             in vec3 vertex_normal;
             
             out vec4 out_color;
             
             void main()
             {
             vec3 color = vec3(0);
             
             vec3 surface_color = material_color;
             vec3 surface_position = vertex_position;
             vec3 surface_normal = normalize(vertex_normal);
             
             float ambient = 1;
             vec3 ambient_color = vec3(0.5,0.5,0.5);
             color += ambient * ambient_color * surface_color;
             
             // Non Moving Directional Light
             
             vec3 light_direction = normalize(vec3(-1,-1, 1));
             vec3 to_light = -light_direction;
             vec3 light_color =  vec3(0.4,0.4,0.4);
             
             float diffuse_k = 1;
             float diffuse_intensity = max(0, dot(to_light, surface_normal));
             color += diffuse_k * diffuse_intensity * light_color * surface_color;
             
             vec3 view_dir = vec3(0, 0, -1);    //    Because we are using an orthograpic projection, and because of the direction of the projection
             vec3 halfway_dir = normalize(view_dir + to_light);
             float specular_k = 1;
             //float shininess = 64;
             float specular_intensity = max(0, dot(halfway_dir, surface_normal));
             color += specular_k * pow(specular_intensity, 64) * light_color;
             
             vec3 point_light_position = (vec3(u_mouse_position, -1));
             vec3 point_light_color= vec3(0.5, 0.5, 0.5);
             vec3 to_point_light = normalize(point_light_position - surface_position);
             
             diffuse_k= 1;
             diffuse_intensity = max(0, dot(to_point_light, surface_normal));
             color += diffuse_k * diffuse_intensity * point_light_color * surface_color;
                                             
             halfway_dir = normalize(view_dir + to_point_light);
             specular_k = 1;
             specular_intensity= pow(max(0, dot(halfway_dir, surface_normal)), shininess);
             color += specular_k * specular_intensity * point_light_color;
                                            
             
             out_color = vec4(color, 1);
             }
             )FRAGMENT"
             );
            break;
        
        case 5: // Small Chase Game
            shader_program = CreateProgramFromSources
            (
             R"VERTEX(
             #version 330 core
             
             layout(location = 0) in vec3 a_position;
             layout(location = 1) in vec3 a_normal;
             
             uniform mat4 u_transform;
             
             out vec3 vertex_position;
             out vec3 vertex_normal;
             
             void main()
             {
             gl_Position = u_transform * vec4(a_position, 1);
             vertex_normal = vec3(u_transform * vec4(a_normal, 0));
             vertex_position = vec3(gl_Position);
             }
             )VERTEX",
             
             R"FRAGMENT(
             #version 330 core
             
             uniform vec2 u_mouse_position;
             uniform vec3 material_color;
             in vec3 vertex_position;
             in vec3 vertex_normal;
             
             out vec4 out_color;
             
             void main()
             {
             vec3 color = vec3(0);
             
             vec3 surface_color = material_color;
             vec3 surface_position = vertex_position;
             vec3 surface_normal = normalize(vertex_normal);
             
             float ambient = 1;
             vec3 ambient_color = vec3(0.5,0.5,0.5);
             color += ambient * ambient_color * surface_color;
             
             // Non Moving Directional Light
             
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
             
             // Point Light
             vec3 point_light_position = (vec3(0,0, -1));
             vec3 point_light_color= vec3(0.5, 0.5, 0.5);
             vec3 to_point_light = normalize(point_light_position - surface_position);
             
             diffuse_k= 1;
             diffuse_intensity = max(0, dot(to_point_light, surface_normal));
             color += diffuse_k * diffuse_intensity * point_light_color * surface_color;
                                             
             halfway_dir = normalize(view_dir + to_point_light);
             specular_k = 1;
             specular_intensity= pow(max(0, dot(halfway_dir, surface_normal)), shininess);
             color += specular_k * specular_intensity * point_light_color;
             
             out_color = vec4(color, 1);
             }
             )FRAGMENT"
             );
            break;
        case 6:
            shader_program = CreateProgramFromSources
            (
             R"VERTEX(
             #version 330 core
             
             layout(location = 0) in vec3 a_position;
             layout(location = 1) in vec3 a_normal;
             
             uniform mat4 u_transform;
             uniform float time;
             uniform float noise_k;
             uniform bool noise_mode;
             
             out vec3 init_position;
             out vec3 vertex_position;
             out vec3 vertex_normal;
             out float displacement;
             
             float noise;
            vec2 vUv;
              vec3 mod289(vec3 x)
              {
                return x - floor(x * (1.0 / 289.0)) * 289.0;
              }

              vec4 mod289(vec4 x)
              {
                return x - floor(x * (1.0 / 289.0)) * 289.0;
              }

              vec4 permute(vec4 x)
              {
                return mod289(((x*34.0)+1.0)*x);
              }

              vec4 taylorInvSqrt(vec4 r)
              {
                return 1.79284291400159 - 0.85373472095314 * r;
              }

              vec3 fade(vec3 t) {
                return t*t*t*(t*(t*6.0-15.0)+10.0);
              }
             float pnoise(vec3 P, vec3 rep)
             {
               vec3 Pi0 = mod(floor(P), rep); // Integer part, modulo period
               vec3 Pi1 = mod(Pi0 + vec3(1.0), rep); // Integer part + 1, mod period
               Pi0 = mod289(Pi0);
               Pi1 = mod289(Pi1);
               vec3 Pf0 = fract(P); // Fractional part for interpolation
               vec3 Pf1 = Pf0 - vec3(1.0); // Fractional part - 1.0
               vec4 ix = vec4(Pi0.x, Pi1.x, Pi0.x, Pi1.x);
               vec4 iy = vec4(Pi0.yy, Pi1.yy);
               vec4 iz0 = Pi0.zzzz;
               vec4 iz1 = Pi1.zzzz;

               vec4 ixy = permute(permute(ix) + iy);
               vec4 ixy0 = permute(ixy + iz0);
               vec4 ixy1 = permute(ixy + iz1);

               vec4 gx0 = ixy0 * (1.0 / 7.0);
               vec4 gy0 = fract(floor(gx0) * (1.0 / 7.0)) - 0.5;
               gx0 = fract(gx0);
               vec4 gz0 = vec4(0.5) - abs(gx0) - abs(gy0);
               vec4 sz0 = step(gz0, vec4(0.0));
               gx0 -= sz0 * (step(0.0, gx0) - 0.5);
               gy0 -= sz0 * (step(0.0, gy0) - 0.5);

               vec4 gx1 = ixy1 * (1.0 / 7.0);
               vec4 gy1 = fract(floor(gx1) * (1.0 / 7.0)) - 0.5;
               gx1 = fract(gx1);
               vec4 gz1 = vec4(0.5) - abs(gx1) - abs(gy1);
               vec4 sz1 = step(gz1, vec4(0.0));
               gx1 -= sz1 * (step(0.0, gx1) - 0.5);
               gy1 -= sz1 * (step(0.0, gy1) - 0.5);

               vec3 g000 = vec3(gx0.x,gy0.x,gz0.x);
               vec3 g100 = vec3(gx0.y,gy0.y,gz0.y);
               vec3 g010 = vec3(gx0.z,gy0.z,gz0.z);
               vec3 g110 = vec3(gx0.w,gy0.w,gz0.w);
               vec3 g001 = vec3(gx1.x,gy1.x,gz1.x);
               vec3 g101 = vec3(gx1.y,gy1.y,gz1.y);
               vec3 g011 = vec3(gx1.z,gy1.z,gz1.z);
               vec3 g111 = vec3(gx1.w,gy1.w,gz1.w);

               vec4 norm0 = taylorInvSqrt(vec4(dot(g000, g000), dot(g010, g010), dot(g100, g100), dot(g110, g110)));
               g000 *= norm0.x;
               g010 *= norm0.y;
               g100 *= norm0.z;
               g110 *= norm0.w;
               vec4 norm1 = taylorInvSqrt(vec4(dot(g001, g001), dot(g011, g011), dot(g101, g101), dot(g111, g111)));
               g001 *= norm1.x;
               g011 *= norm1.y;
               g101 *= norm1.z;
               g111 *= norm1.w;

               float n000 = dot(g000, Pf0);
               float n100 = dot(g100, vec3(Pf1.x, Pf0.yz));
               float n010 = dot(g010, vec3(Pf0.x, Pf1.y, Pf0.z));
               float n110 = dot(g110, vec3(Pf1.xy, Pf0.z));
               float n001 = dot(g001, vec3(Pf0.xy, Pf1.z));
               float n101 = dot(g101, vec3(Pf1.x, Pf0.y, Pf1.z));
               float n011 = dot(g011, vec3(Pf0.x, Pf1.yz));
               float n111 = dot(g111, Pf1);

               vec3 fade_xyz = fade(Pf0);
               vec4 n_z = mix(vec4(n000, n100, n010, n110), vec4(n001, n101, n011, n111), fade_xyz.z);
               vec2 n_yz = mix(n_z.xy, n_z.zw, fade_xyz.y);
               float n_xyz = mix(n_yz.x, n_yz.y, fade_xyz.x);
               return 2.2 * n_xyz;
             }
             

             float turbulence( vec3 p ) {

               float w = 10.0;
               float t = -.5;

               for (float f = 1.0 ; f <= 10.0 ; f++ )
               {
                 float power = pow( 2.0, f );
                 t += abs( pnoise( vec3( power * p ), vec3( 10.0, 10.0, 10.0 ) ) / power );
                }

               return t;

             }
             
             void main()
             {
             init_position = vec3(vec4(a_position,1) * u_transform);
             noise = 10.0 *  -.10 * turbulence( abs(500*sin(time/1000))/5.0 * .5 * a_normal );
             float b = 5.0 * pnoise( 0.05 * a_position, vec3( 100.0 ) );
             displacement = - 10. * noise + b;
             vec3 newPosition = a_position + a_normal * displacement * noise_k;
             
             gl_Position = u_transform * vec4(newPosition,1);
             vertex_normal = vec3(u_transform * vec4(a_normal, 0));
             vertex_position = vec3(gl_Position);
             }
             )VERTEX",
             
             R"FRAGMENT(
             #version 330 core
             
             uniform vec2 u_mouse_position;
             uniform vec3 material_color;
             uniform float time;
             uniform float experiment;
             in vec3 vertex_position;
             in vec3 vertex_normal;
             in vec3 init_position;
             in float displacement;
             
             out vec4 out_color;
             
             void main()
             {
             vec3 color = vec3(0);
             
             vec2 pos = 10.0 * (vertex_normal.xy / 2.0);
             float distance;
             for (int i = 0; i < 20; i++)
             {
                distance = length(vec2(pos.x, pos.y));

                 pos.x = pos.x - cos(pos.y + sin(distance));
                 pos.x += time / 2.0;
                 pos.y = pos.y + sin(pos.x + cos(distance));
                 pos.y += time / 12.0;
             }
             
             vec3 surface_color;
             if(experiment == 1)
                surface_color = vec3(cos(distance) + 1.9, cos(distance*0.2) + 1.0, cos(distance*0.2) + 1.9) * 0.3;
             else
             {
                float darken = 1.0/pow(displacement + 3,1.0/1.0);
                surface_color = vec3(0.4);
                surface_color *= darken;
             }

             vec3 surface_position = vertex_position;
             vec3 surface_normal = normalize(vertex_normal);
             
             float ambient = 0.0;
             if(experiment == 1)
                ambient = 0.7;
             vec3 ambient_color = vec3(0.5);
             color += ambient * ambient_color * surface_color;
             
             // Point Light 1
             
             vec3 light_direction = normalize(vec3(-1,-1, 1));
             vec3 to_light = -light_direction;
             vec3 light_color =  vec3(0.7,0.9,1);
             if(experiment == 1)
                light_color =  vec3(1,1,1);
             
             float diffuse_k = 1;
             float diffuse_intensity = max(0, dot(to_light, surface_normal));
             color += diffuse_k * diffuse_intensity * light_color * surface_color / (length(to_light) * length(surface_normal));
             
             vec3 view_dir = vec3(0, 0, -1);    //    Because we are using an orthograpic projection, and because of the direction of the projection
             vec3 halfway_dir = normalize(view_dir + to_light);
             float specular_k = 1;
             float shininess = 64;
             float specular_intensity = max(0, dot(halfway_dir, surface_normal));
             color += specular_k * pow(specular_intensity, shininess) * light_color;
             
             // Point Light 2
             if(experiment == 0)
             {
                 light_direction = normalize(vec3(6,6, 1));
                 to_light = -light_direction;
                 light_color =  vec3(1,0.7,1);
                 
                 diffuse_k = 1;
                 diffuse_intensity = max(0, dot(to_light, surface_normal));
                 color += diffuse_k * diffuse_intensity * light_color * surface_color / (length(to_light) * length(surface_normal));
                 
                 view_dir = vec3(0, 0, -1);    //    Because we are using an orthograpic projection, and because of the direction of the projection
                 halfway_dir = normalize(view_dir + to_light);
                 specular_k = 1;
                 shininess = 64;
                 specular_intensity = max(0, dot(halfway_dir, surface_normal));
                 color += specular_k * pow(specular_intensity, shininess) * light_color;
             }
             
             out_color = vec4(color, 1);
             }
             )FRAGMENT"
             );
            break;
        default:
            break;
    }
    
    if (shader_program == NULL)
    {
        glfwTerminate();
    }
    
    Globals.u_mouse_location = glGetUniformLocation(shader_program, "u_mouse_position");
    Globals.u_transform_location = glGetUniformLocation(shader_program, "u_transform");
    Globals.material_color = glGetUniformLocation(shader_program, "material_color");
    Globals.time = glGetUniformLocation(shader_program, "time");
    Globals.noise_k = glGetUniformLocation(shader_program, "noise_k");
    Globals.experiment_shader = glGetUniformLocation(shader_program, "experiment");
    Globals.shininess = glGetUniformLocation(shader_program, "shininess");
    
    glUseProgram(shader_program); // use shader
}

// Scenes
void Scene1234(const VAO & sphereVAO, const VAO & donutVAO, const VAO & spikesVAO, const VAO & experimentVAO)
{
    // DRAW THE OBJECTS
    
    
    
    // Sphere
    glm::mat4 transform(1.0);
    transform = glm::translate(transform, glm::vec3(-0.5, 0.5, 0));
    transform = glm::scale(transform, glm::vec3(0.4));
    transform = glm::rotate(transform, glm::radians(float(glfwGetTime()*Globals.rotation_speed)), glm::vec3(1, 1, 0));
    glUniformMatrix4fv(Globals.u_transform_location, 1, GL_FALSE, glm::value_ptr(transform));
    float color1[3] = {0.5, 0.5, 0.5};
    glUniform3fv(Globals.material_color, 1, color1);
    glUniform1f(Globals.shininess, 128); // set shininess
    
    glBindVertexArray(sphereVAO.id);
    glDrawElements(GL_TRIANGLES, sphereVAO.element_array_count, GL_UNSIGNED_INT, NULL);
    
    
    // Donut
    transform = glm::mat4(1.0);
    transform = glm::translate(transform, glm::vec3(0.5, 0.5, 0));
    transform = glm::scale(transform, glm::vec3(0.4));
    transform = glm::rotate(transform, glm::radians(float(glfwGetTime()*Globals.rotation_speed)), glm::vec3(1, 1, 0));
    glUniformMatrix4fv(Globals.u_transform_location, 1, GL_FALSE, glm::value_ptr(transform));
    float color2[3] = {1.0, 0.0, 0.0};
    glUniform3fv(Globals.material_color, 1, color2);
    glUniform1f(Globals.shininess, 32); // set shininess
    
    glBindVertexArray(donutVAO.id);
    glDrawElements(GL_TRIANGLES, donutVAO.element_array_count, GL_UNSIGNED_INT, NULL);
    
    // Spikes
    transform = glm::mat4(1.0);
    transform = glm::translate(transform, glm::vec3(-0.5, -0.5, 0));
    transform = glm::scale(transform, glm::vec3(0.4));
    transform = glm::rotate(transform, glm::radians(float(glfwGetTime()*Globals.rotation_speed)), glm::vec3(1, 1, 0));
    glUniformMatrix4fv(Globals.u_transform_location, 1, GL_FALSE, glm::value_ptr(transform));
    float color3[3] = {0.0, 1.0, 0.0};
    glUniform3fv(Globals.material_color, 1, color3);
    glUniform1f(Globals.shininess, 128); // set shininess
    
    glBindVertexArray(spikesVAO.id);
    glDrawElements(GL_TRIANGLES, spikesVAO.element_array_count, GL_UNSIGNED_INT, NULL);
    
    
    // Experiment
    transform = glm::mat4(1.0);
    transform = glm::translate(transform, glm::vec3(0.5, -0.5, 0));
    transform = glm::scale(transform, glm::vec3(0.4));
    transform = glm::rotate(transform, glm::radians(float(glfwGetTime()*Globals.rotation_speed)), glm::vec3(1, 1, 0));
    glUniformMatrix4fv(Globals.u_transform_location, 1, GL_FALSE, glm::value_ptr(transform));
    
    float color4[3] = {0.0, 0.0, 1.0};
    glUniform3fv(Globals.material_color, 1, color4);
    glUniform1f(Globals.shininess, 128); // set shininess
    
    glBindVertexArray(experimentVAO.id);
    glDrawElements(GL_TRIANGLES, experimentVAO.element_array_count, GL_UNSIGNED_INT, NULL);
}
void Scene5(const VAO & sphereVAO, const glm::dvec2 & mouse_position, glm::dvec3 & chasing_pos)
{
    // Police
    chasing_pos = glm::mix(glm::dvec3(mouse_position,0), chasing_pos, 0.99); // update police position
    
    glm::mat4 transform(1.0);
    transform = glm::translate(transform, glm::vec3(chasing_pos.x, chasing_pos.y, 0));
    transform = glm::scale(transform, glm::vec3(0.3));
    transform = glm::rotate(transform, glm::radians(float(glfwGetTime()*Globals.rotation_speed)), glm::vec3(1, 1, 0));
    glUniformMatrix4fv(Globals.u_transform_location, 1, GL_FALSE, glm::value_ptr(transform));
    
    glm::fvec3 color = glm::fvec3(0.5,0.5,0.5);
    glUniform3fv(Globals.material_color, 1, glm::value_ptr(color));
    
    glBindVertexArray(sphereVAO.id);
    glDrawElements(GL_TRIANGLES, sphereVAO.element_array_count, GL_UNSIGNED_INT, NULL);
    
    // Thief
    transform = glm::mat4(1.0);
    transform = glm::translate(transform, glm::vec3(mouse_position, 0));
    transform = glm::scale(transform, glm::vec3(0.3));
    transform = glm::rotate(transform, glm::radians(float(glfwGetTime()*Globals.rotation_speed)), glm::vec3(1, 1, 0));
    glUniformMatrix4fv(Globals.u_transform_location, 1, GL_FALSE, glm::value_ptr(transform));
    
    if( glm::distance(glm::dvec3(mouse_position,0), chasing_pos) <= 0.6 )
        color = glm::fvec3(1,0,0);
    else
        color = glm::fvec3(0,1,0);
        
    glUniform3fv(Globals.material_color, 1, glm::value_ptr(color));
    
    glBindVertexArray(sphereVAO.id);
    glDrawElements(GL_TRIANGLES, sphereVAO.element_array_count, GL_UNSIGNED_INT, NULL);
}

void Scene6(const VAO & experimentVAO)
{
    if(Globals.noise_mode == 1 && Globals.noise_k_val <= Globals.noise_max)
    {
        Globals.noise_k_val += Globals.noise_change_k;
    }
    else if(Globals.noise_mode == 2 && Globals.noise_k_val >= 0)
    {
        Globals.noise_k_val -= Globals.noise_change_k;
    }
    
    glUniform1f(Globals.time, glfwGetTime() - Globals.start_time);
    glUniform1f(Globals.noise_k, Globals.noise_k_val);
    glUniform1f(Globals.experiment_shader, Globals.experiment);
    
    
    // Experiment
    glm::mat4 transform =  glm::mat4(1.0);
    transform = glm::translate(transform, glm::vec3(0, 0, 0));
    transform = glm::scale(transform, glm::vec3(0.8));
    transform = glm::rotate(transform, glm::radians(float(glfwGetTime()*Globals.rotation_speed)), glm::vec3(1, 1, 0));
    glUniformMatrix4fv(Globals.u_transform_location, 1, GL_FALSE, glm::value_ptr(transform));
    float color4[3] = {0.0, 1.0, 0.0};
    glUniform3fv(Globals.material_color, 1, color4);
    
    glBindVertexArray(experimentVAO.id);
    glDrawElements(GL_TRIANGLES, experimentVAO.element_array_count, GL_UNSIGNED_INT, NULL);
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
    glfwSetKeyCallback(window, key_callback);
    
    /* Configure OpenGL */
    glClearColor(0, 0, 0, 1);
    glEnable(GL_DEPTH_TEST);
    
    /* Creating Meshes */
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<GLuint> indices;
    
    // Donut
    GenerateParametricShapeFrom2D(positions, normals, indices, ParametricCircle, 16, 16, false);
    VAO donutVAO(positions, normals, indices);
    {positions.clear();normals.clear();indices.clear();} // clear the attributes
    
    // Sphere
    GenerateParametricShapeFrom2D(positions, normals, indices, ParametricHalfCircle, 16, 16, false);
    VAO sphereVAO(positions, normals, indices);
    {positions.clear();normals.clear();indices.clear();} // clear the attributes
    
    // Sphere
    GenerateParametricShapeFrom2D(positions, normals, indices, ParametricHalfCircle, 256, 256, false);
    VAO sphere2VAO(positions, normals, indices);
    {positions.clear();normals.clear();indices.clear();} // clear the attributes
        
    // Spike
    GenerateParametricShapeFrom2D(positions, normals, indices, ParametricSpikes, 64, 64, false);
    VAO spikesVAO(positions, normals, indices);
    {positions.clear();normals.clear();indices.clear();} // clear the attributes
    
    // Experiment
    GenerateParametricShapeFrom2D(positions, normals, indices, ParametricExperiment, 64, 64, true);
    VAO experimentVAO(positions, normals, indices);
    {positions.clear();normals.clear();indices.clear();} // clear the attributes
    
        
    // Shader Temp
    
    GLuint shader_program = CreateProgramFromSources
    (
     R"VERTEX(
     #version 330 core
     
     layout(location = 0) in vec3 a_position;
     
     uniform mat4 u_transform;
     
     void main()
     {
     gl_Position = u_transform * vec4(a_position, 1);
     }
     )VERTEX",
     
     R"FRAGMENT(
     #version 330 core
     
     out vec4 out_color;
     
     void main()
     {
     out_color = vec4(1,1,1, 1); // only white
     }
     )FRAGMENT"
     );
    
    if (shader_program == NULL)
    {
        glfwTerminate();
        return -1;
    }
    
    Globals.u_transform_location = glGetUniformLocation(shader_program, "u_transform");
    glUseProgram(shader_program); // use shader
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    glm::dvec3 chase_position = glm::dvec3(0,0,0);
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        /* Polygon Mode */
        if (Globals.scene == 1)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // to make it polygon mode scene 1
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // to make it polygon mode scene 1
        
        // Change the position of a vertex dynamically
        auto mouse_position = Globals.mouse_position / glm::dvec2(Globals.screen_dimensions);
        mouse_position.y = 1. - mouse_position.y;
        mouse_position = mouse_position * 2. - 1.;
        
        // Light
        glUniform2fv(Globals.u_mouse_location, 1, glm::value_ptr(glm::vec2(mouse_position))); // light position
        
        /* SCENES */
        
        // Scene 1, 2, 3, 4
        if(Globals.scene >= 1 && Globals.scene <= 4)
            Scene1234(sphereVAO, donutVAO, spikesVAO, experimentVAO);
        else if(Globals.scene == 5)
            Scene5(sphereVAO, mouse_position, chase_position);
        else if(Globals.scene == 6)
        {
            if(Globals.experiment)
                Scene6(experimentVAO);
            else
                Scene6(sphere2VAO);
        }
            
        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        
        /* Poll for and process events */
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}


