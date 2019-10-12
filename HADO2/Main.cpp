#include <Siv3D.hpp>

class Ball {
public:
	Circle circle;
	Color color;
	Vec2 velocity;
	Vec2 beforecenter;
	bool reflect;
	void draw() {
		circle.draw(color);
	}
	void hado(Vec2& center, int& power) {
		double a = atan2(circle.y - center.y, circle.x - center.x);
		Vec2 b;
		b.x = cos(a) * 1.1 * power / circle.r;
		b.y = sin(a) * 1.1 * power / circle.r;
		velocity += b;
	}
	void move() {
		beforecenter = circle.center;
		circle.center += velocity;
	}
	void attenuate() {
		velocity *= 0.95;
	}
	bool across(Rect r) {
		return(circle.center.distanceFrom(beforecenter) > 3 && r.intersects(Line(circle.center, beforecenter)) || r.intersects(circle));
	}
};

class PlayerBall : public Ball {
public:
	void cursor() {
		if (circle.center.distanceFrom(Cursor::Pos()) < 10) {
			velocity *= 0.9;
		}
		else {
			double a = atan2(Cursor::Pos().y - circle.y, Cursor::Pos().x - circle.x);
			Vec2 b;
			b.x = cos(a) * 0.4;
			b.y = sin(a) * 0.4;
			velocity += b;
		}
	}
};

class Hado {
public:
	Vec2 center;
	int radius, power;
	bool hitred, hitblue, hitwhite;
	Color color;
	void move() {
		radius += 15;
		power -= 5;
	}
	void draw() {
		Color c = color;
		c.a = 0;
		for (int i = 0; i < power; i++) {
			color.a = 255 * i / (power * 2) + 80;
			Circle(center, radius + power - i).drawFrame(1, color);
		}
	}
	void hit(PlayerBall& Red, Ball& Blue,Ball& White) {
		if (!hitred && Circle(center, radius + power).intersects(Red.circle)) {
			hitred = true;
			Red.hado(center, power);
		}
		if (!hitblue && Circle(center, radius + power).intersects(Blue.circle)) {
			hitblue = true;
			Blue.hado(center, power);
		}
		if (!hitwhite && Circle(center, radius + power).intersects(White.circle)) {
			hitwhite = true;
			White.hado(center, power);
		}
	}
};


void init(PlayerBall& Red, Ball& Blue, Ball& White, int& RedHado, int& BlueHado) {
	Red.circle.center = Red.beforecenter = {250, 225};
	Red.velocity = {0, 0};
	Red.circle.r = 10;
	Red.color = Palette::Red;
	
	Blue.circle.center = Blue.beforecenter = {550, 225};
	Blue.velocity = {0, 0};
	Blue.circle.r = 10;
	Blue.color = Palette::Blue;

	White.circle.center = White.beforecenter = {400, 225};
	White.velocity = {0, 0};
	White.circle.r = 4;
	White.color = Palette::White;

	RedHado = BlueHado = 0;
}

void Main() {
	Scene::SetBackground(Palette::Black);
	Graphics::SetTargetFrameRateHz(60);
	const Array<Rect> FieldTop {
		Rect(50, 170, 50, 5), Rect(700, 170, 50, 5), Rect(95, 45, 610, 5)
	};
	const Array<Rect> FieldBottom {
		Rect(50, 275, 50, 5), Rect(700, 275, 50, 5), Rect(95, 400, 610, 5)
	};
	const Array<Rect> FieldLeft{
		Rect(95, 45, 5, 130), Rect(95, 275, 5, 130), Rect(50, 170, 5, 110)
	};
	const Array<Rect> FieldRight{
		Rect(700, 45, 5, 130), Rect(700, 275, 5, 130), Rect(745, 170, 5, 110)
	};
	const Array<Rect> FieldTopInvisible{
		Rect(-200, 170, 300, 5), Rect(700, 170, 300, 5), Rect(-200, 45, 1200, 5)
	};
	const Array<Rect> FieldBottomInvisible{
		Rect(-200, 275, 300, 5), Rect(700, 275, 300, 5), Rect(-200, 400, 1200, 5)
	};
	const Array<Rect> FieldLeftInvisible{
		Rect(95, -200, 5, 375), Rect(95, 275, 5, 380), Rect(50, 0, 5, 450)
	};
	const Array<Rect> FieldRightInvisible{
		Rect(700, -200, 5, 375), Rect(700, 275, 5, 380), Rect(745, 0, 5, 450)
	};

	const Rect RedGoal(55, 175, 45, 100);
	Array<Hado> HadoArray;
	PlayerBall Red;
	Ball Blue, White;
	int RedHado, BlueHado;
	init(Red, Blue, White, RedHado, BlueHado);
	while (System::Update()) {
		RedGoal.draw({ Color(255,0,0,255),Color(255,0,0,50),Color(255,0,0,50),Color(255,0,0,255) });
		if (KeySpace.down()) {
			init(Red, Blue, White, RedHado, BlueHado);
		}
		if (MouseR.down()) {
			White.circle.center = Cursor::Pos();
		}
		Circle(Cursor::Pos(), 5).draw(Color(255, 0, 0, 100));
		Red.reflect = Blue.reflect = White.reflect = false;
		for (const Rect& r : FieldTop) {
			r.draw();

		}for (const Rect& r : FieldBottom) {
			r.draw();
		}
		for (const Rect& r : FieldLeft) {
			r.draw();
		}
		for (const Rect& r : FieldRight) {
			r.draw();
		}
		do {
			Red.move();
			Red.reflect = false;
			for (const Rect& r : FieldTop) {
				if (Red.across(r)) {
					Red.velocity.y *= -1.1;
					Red.circle.y = r.y + r.h + Red.circle.r + 1;
					Red.reflect = true;
					break;
				}
			}
			for (const Rect& r : FieldBottom) {
				if (Red.across(r)) {
					Red.velocity.y *= -1.1;
					Red.circle.y = r.y - Red.circle.r - 1;
					Red.reflect = true;
					break;
				}
			}
			for (const Rect& r : FieldLeft) {
				if (Red.across(r)) {
					Red.velocity.x *= -1.1;
					Red.circle.x = r.x + r.w + Red.circle.r + 1;
					Red.reflect = true;
					break;
				}
			}
			for (const Rect& r : FieldRight) {
				if (Red.across(r)) {
					Red.velocity.x *= -1.1;
					Red.circle.x = r.x - Red.circle.r - 1;
					Red.reflect = true;
					break;
				}
			}
		} while (Red.reflect);
		do {
			Blue.move();
			Blue.reflect = false;
			for (const Rect& r : FieldTop) {
				if (Blue.across(r)) {
					Blue.velocity.y *= -1.1;
					Blue.circle.y = r.y + r.h + Blue.circle.r + 1;
					Blue.reflect = true;
					break;
				}
			}
			for (const Rect& r : FieldBottom) {
				if (Blue.across(r)) {
					Blue.velocity.y *= -1.1;
					Blue.circle.y = r.y - Blue.circle.r - 1;
					Blue.reflect = true;
					break;
				}
			}
			for (const Rect& r : FieldLeft) {
				if (Blue.across(r)) {
					Blue.velocity.x *= -1.1;
					Blue.circle.x = r.x + r.w + Blue.circle.r + 1;
					Blue.reflect = true;
					break;
				}
			}
			for (const Rect& r : FieldRight) {
				if (Blue.across(r)) {
					Blue.velocity.x *= -1.1;
					Blue.circle.x = r.x - Blue.circle.r - 1;
					Blue.reflect = true;
					break;
				}
			}
		} while (Blue.reflect);
		do {
			White.move();
			White.reflect = false;
			for (const Rect& r : FieldTop) {
				if (White.across(r)) {
					White.velocity.y *= -1.1;
					White.circle.y = r.y + r.h + White.circle.r + 1;
					White.reflect = true;
					break;
				}
			}
			for (const Rect& r : FieldBottom) {
				if (White.across(r)) {
					White.velocity.y *= -1.1;
					White.circle.y = r.y - White.circle.r - 1;
					White.reflect = true;
					break;
				}
			}
			for (const Rect& r : FieldLeft) {
				if (White.across(r)) {
					White.velocity.x *= -1.1;
					White.circle.x = r.x + r.w + White.circle.r + 1;
					White.reflect = true;
					break;
				}
			}
			for (const Rect& r : FieldRight) {
				if (White.across(r)) {
					White.velocity.x *= -1.1;
					White.circle.x = r.x - White.circle.r - 1;
					White.reflect = true;
					break;
				}
			}
		} while (White.reflect);
		for (Hado& h : HadoArray) {
			h.draw();
			h.hit(Red, Blue, White);
			h.move();
		}
		while (!HadoArray.empty() && HadoArray.front().power <= 0) {
			HadoArray.pop_front();
		}
		if (MouseL.pressed()) {
			RedHado++;
		}
		if (MouseL.up()) {
			HadoArray.push_back({ Red.circle.center, 0, RedHado, true, false, false, Palette::Red });
			RedHado = 0;
			//ゆっくり減るよう あとで調整
		}
		RedHado = Min(RedHado, 240);

		Red.draw();
		Blue.draw();
		White.draw();
		Red.cursor();
		Red.attenuate();
		Blue.attenuate();
		White.attenuate();

		Print << White.circle.center << White.velocity;
		Rect(120, 420, 240, 40).drawFrame(0, 2, Color(255, 200, 200));
		Rect(120, 420, RedHado, 40).draw(Palette::Red);
		Rect(440, 420, 240, 40).drawFrame(0, 2, Color(200, 200, 255));
		Rect(680 - BlueHado, 420, BlueHado, 40).draw(Palette::Blue);
	}
}