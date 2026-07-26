export module globals;

export namespace globals {
    const int api_server_port {8888};
    const int llm_server_port {8889};

    const float drone_acceptance_radius_m = 0.5f;
    const float drone_takeoff_altitude_m = 10.f;
}
