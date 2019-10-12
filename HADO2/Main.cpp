#include <Siv3D.hpp>

class Ball {
public:
	Circle circle;
	Color color;
	Vec2 point;
	Vec2 velocity;
	void Draw() {
		circle.center = point;
		circle.draw(color);
	}
	void accelerate(Vec2 a) {
		if (a.distanceFrom(point) < 5) {
			velocity *= 0.9;
		}
		else {
			Vec2 b;
			b.x = cos(atan2(a.y - point.y, a.x - point.x)) * 0.6;
			b.y = sin(atan2(a.y - point.y, a.x - point.x)) * 0.6;
			velocity += b;
		}
	}
	void move() {
		point += velocity;
	}
	void attenuate() {
		velocity *= 0.95;
	}
};

void init(Ball& Red, Ball& Blue, Ball& White, double& RedHado, double& BlueHado) {
	Red.point = { 300,200 };
	Red.circle.r = 10;
	Red.color = Palette::Red;
}

void Main() {
	Scene::SetBackground(Palette::Black);
	Graphics::SetTargetFrameRateHz(60);
	const Font font(60);

	const Polygon Field{
		Vec2(100,50),Vec2(700,50),Vec2(700,175),Vec2(740,175),Vec2(740,275),Vec2(700,275),Vec2(700,400),Vec2(100,400),Vec2(100,275),Vec2(60,275),Vec2(60,175),Vec2(100,175)
	};
	Ball Red, Blue, White;
	double RedHado, BlueHado;
	init(Red, Blue, White, RedHado, BlueHado);
	while (System::Update()) {
		Field.draw(Palette::Black).drawFrame(5);
		Red.accelerate(Cursor::Pos());
		Red.move();
		Red.Draw();
		Red.attenuate();
		Circle(Cursor::Pos(), 5).draw(Color(255, 0, 0, 100));
	}
}