#define is_down(b) input->buttons[b].is_down
#define pressed(b) (input->buttons[b].is_down && input->buttons[b].changed)
#define released(b) (!input->buttons[b].is_down && input->buttons[b].changed)

float player1_p, player2_p; //positiion
float player1_dp, player2_dp; //derivative of player_p (velocity)
float arena_half_size_x = 85, arena_half_size_y = 45;
float player_half_size_x = 2.5, player_half_size_y = 12;
float ball_p_x, ball_p_y, ball_dp_x = 100, ball_dp_y, ball_half_size = 1;

//position,velocity,acceleration,delta time
internal void
simulate_player(float* p, float* dp, float ddp, float dt) {
	ddp -= *dp * 10.f;

	*p = *p + *dp * dt + ddp * dt * dt * .5f;
	*dp = *dp + ddp * dt;

	//collision detection
	if (*p + player_half_size_y > arena_half_size_y) {
		*p = arena_half_size_y - player_half_size_y;
		*dp = 0;
	}
	else if (*p - player_half_size_y < -arena_half_size_y) {
		*p = -arena_half_size_y + player_half_size_y;
		*dp = 0;
	}
}

internal bool
aabb_vs_aabb(float p1x, float p1y, float hs1x, float hs1y,
	float p2x, float p2y, float hs2x, float hs2y) {
	return (p1x + hs1x > p2x - hs2x &&
		p1x - hs1x < p2x + hs2x &&
		p1y + hs1y > p2y - hs2y &&
		p1y + hs1y < p2y + hs2y);
}

internal void
simulate_game(Input* input, float dt) {
	clear_screen(0xff5500);
	draw_rect(0, 0, arena_half_size_x, arena_half_size_y, 0xffaa33);

	//player 1
	float player1_ddp = 0.f; //derivative of player_dp (acceleration)
	if (is_down(BUTTON_UP)) player1_ddp += 2000;
	if (is_down(BUTTON_DOWN)) player1_ddp -= 2000;

	player1_ddp -= player1_dp * 10.f;

	player1_p = player1_p + player1_dp * dt + player1_ddp * dt * dt * .5f;
	player1_dp = player1_dp + player1_ddp * dt;

	//player 2
	float player2_ddp = 0.f; //derivative of player_dp (acceleration)
	if (is_down(BUTTON_W)) player2_ddp += 2000;
	if (is_down(BUTTON_S)) player2_ddp -= 2000;

	simulate_player(&player1_p, &player1_dp, player1_ddp, dt);
	simulate_player(&player2_p, &player2_dp, player2_ddp, dt);

	// Simulate Ball
	{
		ball_p_x += ball_dp_x * dt;
		ball_p_y += ball_dp_y * dt;

		if (aabb_vs_aabb(ball_p_x, ball_p_y, ball_half_size, ball_half_size, 80, player1_p, player_half_size_x, player_half_size_y)) {
			ball_p_x = 80 - player_half_size_x - ball_half_size;
			ball_dp_x *= -1;
			ball_dp_y = (ball_p_y - player1_p) * 2 + player1_dp * .75f;
		}
		else if (aabb_vs_aabb(ball_p_x, ball_p_y, ball_half_size, ball_half_size, -80, player2_p, player_half_size_x, player_half_size_y)) {
			ball_p_x = -80 + player_half_size_x + ball_half_size;
			ball_dp_x *= -1;
			ball_dp_y = (ball_p_y - player2_p) * 2 + player2_dp * .75f;
		}

		if (ball_p_y + ball_half_size > arena_half_size_y) {
			ball_p_y = arena_half_size_y - ball_half_size;
			ball_dp_y *= -1;
		}
		else if (ball_p_y - ball_half_size < -arena_half_size_y) {
			ball_p_y = -arena_half_size_y + ball_half_size;
			ball_dp_y *= -1;
		}

		if (ball_p_x + ball_half_size > arena_half_size_x) {
			ball_dp_x *= -1;
			ball_dp_y = 0;
			ball_p_x = 0;
			ball_p_y = 0;
			//player1_score++;
		}
		else if (ball_p_x - ball_half_size < -arena_half_size_x) {
			ball_dp_x *= -1;
			ball_dp_y = 0;
			ball_p_x = 0;
			ball_p_y = 0;
			//player2_score++;
		}
	}

	// Rendering
	draw_rect(ball_p_x, ball_p_y, ball_half_size, ball_half_size, 0xffffff); //ball
	draw_rect(-80, player1_p, player_half_size_x, player_half_size_y, 0xff0000); //player1
	draw_rect(80, player2_p, player_half_size_x, player_half_size_y, 0xff0000); //player2
}