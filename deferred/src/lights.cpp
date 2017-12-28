#include "main.h"

void sample_t::init_lights() {
    set_additional_lights(15);
}

void light_t::update_position(float t) {
    const position_params_t& p = position_params;
    position = p.zero + p.dx * p.A * sin(p.a * t + p.d) + p.dy * p.B * sin(p.b * t);
}

void sample_t::update_lights() {
    float t = static_cast<float>(clock() * 1.0 / CLOCKS_PER_SEC);

    for (size_t i = 0; i < lights.size(); ++i) {
        lights[i].update_position(t);
    }
}

size_t sample_t::get_additional_lights() {
    return lights.size() - 1;
}

float rnd() {
    return rand() * 1.0f / RAND_MAX;
}

void sample_t::set_additional_lights(size_t count) {
    lights.resize(count);
    for (size_t i = 0; i < lights.size(); ++i) {
        vec3 color = vec3(rnd(), rnd(), rnd());
        color /= color.r;
        lights[i] = {
            vec3(rnd(), rnd(), rnd()),
            0.6f,
            vec3(),
            color * 1.0f,
            color * 1.0f,
            true,
            {
                    1, 1, lights_speed * rnd(), lights_speed * rnd(),
                    static_cast<float>(M_PI / 2.0f * rnd()),
                    vec3(-rnd() / 2.0, lights_height, rnd() / 2.0),
                    vec3(0.5, 0, 0),
                    vec3(0, 0, -1.0),
            }
        };
    }
}

void TW_CALL sample_t::get_lights_callback(void *value, void *clientData) {
    *static_cast<unsigned*>(value) = static_cast<sample_t*>(clientData)->get_additional_lights();
}

void TW_CALL sample_t::set_lights_callback(const void *value, void *clientData) {
    static_cast<sample_t*>(clientData)->set_additional_lights(*static_cast<const unsigned*>(value));
}
