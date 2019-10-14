#include <Siv3D.hpp>

class Ball {
public:
	Circle circle;
	Color color;
	Vec2 velocity;
	Vec2 beforecenter;
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
	void attenuate() {
		velocity *= 0.95;
	}
	bool across(Rect r) {
		return(circle.center.distanceFrom(beforecenter) > 3 && r.intersects(Line(circle.center, beforecenter)) || r.intersects(circle));
	}
	void move(const Array<Rect>& FieldTopInvisible, const Array<Rect>& FieldBottomInvisible, const Array<Rect>& FieldLeftInvisible, const Array<Rect>& FieldRightInvisible) {
		beforecenter = circle.center;
		circle.center += velocity;
		bool reflect = false;

		for (const Rect& r : FieldTopInvisible) {
			if (across(r)) {
				velocity.y *= -1.1;
				circle.y = r.y + r.h + circle.r + 1;
				reflect = true;
				break;
			}
		}
		for (const Rect& r : FieldBottomInvisible) {
			if (across(r)) {
				velocity.y *= -1.1;
				circle.y = r.y - circle.r - 1;
				reflect = true;
				break;
			}
		}
		for (const Rect& r : FieldLeftInvisible) {
			if (across(r)) {
				velocity.x *= -1.1;
				circle.x = r.x + r.w + circle.r + 1;
				reflect = true;
				break;
			}
		}
		for (const Rect& r : FieldRightInvisible) {
			if (across(r)) {
				velocity.x *= -1.1;
				circle.x = r.x - circle.r - 1;
				reflect = true;
				break;
			}
		}
		if (reflect) {
			move(FieldTopInvisible, FieldBottomInvisible, FieldLeftInvisible, FieldRightInvisible);
		}
	}
	void collision(Ball& ball) {
		Circle judgmentcircle;
		judgmentcircle.center = circle.center;
		judgmentcircle.r = circle.r + ball.circle.r;
		if (ball.circle.center.distanceFrom(ball.beforecenter) < 3) {
			if (circle.intersects(ball.circle)) {
				Vec2 a, b, c;
				int m1, m2;
				a = ball.velocity - velocity;
				b = beforecenter - ball.beforecenter;
				if (a.dot(b) > 0) {
					c = b * a.dot(b) / b.length() / b.length();
					ball.velocity -= c;
					m1 = ball.circle.r * ball.circle.r;
					m2 = circle.r * circle.r;
					ball.velocity += c * (m1 - m2) / (m1 + m2);
					velocity += c * 2 * m1 / (m1 + m2);
				}
			}
		}
		else if (const Optional<Array<Vec2>> points = judgmentcircle.intersectsAt(Line(ball.circle.center, ball.beforecenter))) {
			Vec2 collisionpoint(869120, 869120);
			for (const Vec2& point : points.value()) {
				if (collisionpoint.distanceFrom(ball.beforecenter) < point.distanceFrom(ball.beforecenter)) {
					collisionpoint=point;
				}
			}
			Vec2 a, b, c;
			int m1, m2;
			a = ball.velocity - velocity;
			b = circle.center - collisionpoint;
			if (a.dot(b) > 0) {
				c = b * a.dot(b) / b.length() / b.length();
				ball.velocity -= c;
				m1 = ball.circle.r * ball.circle.r;
				m2 = circle.r * circle.r;
				ball.velocity += c * (m1 - m2) / (m1 + m2);
				velocity += c * 2 * m1 / (m1 + m2);
			}
		}
	}
};

class PlayerBall : public Ball {
public:
	void cursor() {
		if (circle.center.distanceFrom(Cursor::Pos()) < 10) {
			velocity *= 0.9;
		}
		else {
			velocity += (Cursor::Pos() - circle.center) / (Cursor::Pos() - circle.center).length() * 0.4;
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
			Circle(center, radius + power - i).drawFrame(2, color);
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


void init(PlayerBall& Red, Ball& Blue, Ball& White, int& RedHado, int& BlueHado, double& RedHadoDisplay, double& BlueHadoDisplay) {
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
	White.circle.r = 6;
	White.color = Palette::White;

	RedHado = 0;
	BlueHado = 0;

	RedHadoDisplay = 0;
	BlueHadoDisplay = 0;
}

void Main() {
	Scene::SetBackground(Color(0, 0, 50));
	Window::SetTitle(U"HADO2");
	Graphics::SetTargetFrameRateHz(60);
	const Font fontMedium60(60, Typeface::Medium);
	const Font fontMedium100(100, Typeface::Medium);
	const Font fontMedium150(150, Typeface::Medium);
	const Font fontMedium200(200, Typeface::Medium);
	const Font fontHeavy70(70, Typeface::Heavy);
	const Font fontHeavy255(255, Typeface::Heavy);
	const Font fontRegular20(20, Typeface::Regular);
	const Font fontRegular30(30, Typeface::Regular);
	const Font fontRegular40(40, Typeface::Regular);
	const Font fontRegular255(255, Typeface::Regular);
	const Polygon PolygonH {
		Vec2(50, 50), Vec2(80, 50), Vec2(80, 135), Vec2(160, 135), Vec2(160, 50), Vec2(190, 50), Vec2(190, 250), Vec2(160, 250), Vec2(160, 165), Vec2(80,165), Vec2(80, 250), Vec2(50, 250)
	};
	const Polygon PolygonA {
		{Vec2(220, 250), Vec2(270, 50), Vec2(310, 50), Vec2(360, 250), Vec2(330, 250), Vec2(310, 170), Vec2(270, 170), Vec2(250, 250)},
		{{Vec2(277.5, 140), Vec2(290, 90), Vec2(302.5, 140)}}
	};
	const Polygon PolygonD {
		{Vec2(390, 50), Vec2(470, 50), Vec2(530, 110), Vec2(530, 190), Vec2(470, 250), Vec2(390, 250)},
		{{Vec2(420, 80), Vec2(460, 80), Vec2(500, 120), Vec2(500, 180), Vec2(460, 220), Vec2(420, 220)}}
	};
	const Polygon PolygonO {
		{Vec2(590, 50), Vec2(650, 50), Vec2(690, 90), Vec2(690, 210), Vec2(650, 250), Vec2(590, 250), Vec2(550, 210), Vec2(550, 90)},
		{{Vec2(600, 80), Vec2(640, 80), Vec2(660, 100), Vec2(660, 200), Vec2(640, 220), Vec2(600, 220), Vec2(580, 200), Vec2(580, 100)}}
	};
	const Polygon Polygon2{
		Vec2(720, 150), Vec2(780, 150), Vec2(780, 210), Vec2(740, 210), Vec2(740, 230), Vec2(780, 230), Vec2(780, 250), Vec2(720, 250), Vec2(720, 190), Vec2(760, 190), Vec2(760, 170), Vec2(720, 170)
	};
	const Rect InstructionBox(200, 300, 400, 100);
	const Rect PlayBox(200, 450, 400, 100);
	const Rect DifficultyEasyBox(200, 100, 400, 100);
	const Rect DifficultyMediumBox(200, 250, 400, 100);
	const Rect DifficultyHardBox(200, 400, 400, 100);
	const Array<Rect> FieldTop {
		Rect(50, 170, 50, 5), Rect(700, 170, 50, 5), Rect(95, 45, 610, 5)
	};
	const Array<Rect> FieldBottom {
		Rect(50, 275, 50, 5), Rect(700, 275, 50, 5), Rect(95, 400, 610, 5)
	};
	const Array<Rect> FieldLeft {
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
	const Rect BlueGoal(700, 175, 45, 100);
	const Rect RedHadoMeter(120, 420, 240, 40);
	const Rect BlueHadoMeter(440, 420, 240, 40);
	const Rect TimerBox(350, 500, 100, 60);
	Array<Hado> HadoArray;
	PlayerBall Red;
	Ball Blue, White;
	int RedHado, BlueHado, RedPoint, BluePoint, Status, timer, timer2, Difficulty;
	double RedHadoDisplay, BlueHadoDisplay;
	Status = 0;//0:メニュー 1:説明画面 2:難易度選択 10:ゲーム画面 11:赤ゴール 12:青ゴール 13:ゲーム開始前 14:ゲームセット
	RedPoint = 0;
	BluePoint = 0;
	while (System::Update()) {
		if (Status == 0) {
			PolygonH.draw(Palette::Red);
			PolygonH.drawFrame(5, Color(230, 220, 50));
			PolygonA.draw(Palette::Blue);
			PolygonA.drawFrame(5, Color(230, 220, 50));
			PolygonD.draw(Palette::Red);
			PolygonD.drawFrame(5, Color(230, 220, 50));
			PolygonO.draw(Palette::Blue);
			PolygonO.drawFrame(5, Color(230, 220, 50));
			Polygon2.draw(Palette::Red);
			Polygon2.drawFrame(5, Palette::Yellow);
			if (InstructionBox.intersects(Cursor::Pos())) {
				InstructionBox.drawFrame(0, 10, Palette::Yellow);
				fontHeavy70(U"あそびかた").drawAt(InstructionBox.center());
				if (MouseL.down()) {
					Status = 1;
				}
			}
			else {
				InstructionBox.drawFrame(0, 5, Palette::Blue);
				fontMedium60(U"あそびかた").drawAt(InstructionBox.center());
			}
			if (PlayBox.intersects(Cursor::Pos())) {
				PlayBox.drawFrame(0, 10, Palette::Yellow);
				fontHeavy70(U"あそぶ").drawAt(PlayBox.center());
				if (MouseL.down()) {
					Status = 2;
				}
			}
			else {
				PlayBox.drawFrame(0, 5, Palette::Red);
				fontMedium60(U"あそぶ").drawAt(PlayBox.center());
			}
		}
		else if (Status == 1) {
			fontRegular40(U"<ルール>\nサッカーやホッケーみたいなゲームです\n相手のゴールにボールを入れれば1点です\n\nマウス操作で動かします\n左クリック長押しで波動をためます\n左クリックを離すと波動を打てます\n\n").drawAt(400, 300);
			fontRegular30(U"左クリックで戻る").draw(500, 500);
			if (MouseL.down()) {
				Status = 0;
			}
		}
		else if (Status == 2) {
			if (DifficultyEasyBox.intersects(Cursor::Pos())) {
				DifficultyEasyBox.drawFrame(0, 10, Palette::Yellow);
				fontHeavy70(U"よわい").drawAt(DifficultyEasyBox.center());
				if (MouseL.down()) {
					Difficulty = 0;
					init(Red, Blue, White, RedHado, BlueHado, RedHadoDisplay, BlueHadoDisplay);
					timer = 5400;
					timer2 = 240;
					Status = 13;
				}
			}
			else {
				DifficultyEasyBox.drawFrame(0, 5, Palette::Lime);
				fontMedium60(U"よわい").drawAt(DifficultyEasyBox.center());
			}
			if (DifficultyMediumBox.intersects(Cursor::Pos())) {
				DifficultyMediumBox.drawFrame(0, 10, Palette::Yellow);
				fontHeavy70(U"ふつう").drawAt(DifficultyMediumBox.center());
				if (MouseL.down()) {
					Difficulty = 1;
					init(Red, Blue, White, RedHado, BlueHado, RedHadoDisplay, BlueHadoDisplay);
					timer = 5400;
					timer2 = 240;
					Status = 13;
				}
			}
			else {
				DifficultyMediumBox.drawFrame(0, 5, Palette::Orange);
				fontMedium60(U"ふつう").drawAt(DifficultyMediumBox.center());
			}
			if (DifficultyHardBox.intersects(Cursor::Pos())) {
				DifficultyHardBox.drawFrame(0, 10, Palette::Yellow);
				fontHeavy70(U"つよい").drawAt(DifficultyHardBox.center());
				if (MouseL.down()) {
					Difficulty = 2;
					init(Red, Blue, White, RedHado, BlueHado, RedHadoDisplay, BlueHadoDisplay);
					timer = 5400;
					timer2 = 240;
					Status = 13;
				}
			}
			else {
				DifficultyHardBox.drawFrame(0, 5, Palette::Red);
				fontMedium60(U"つよい").drawAt(DifficultyHardBox.center());
			}
			if (KeyT.pressed() && KeyM.pressed() && KeyJ.pressed() && KeyN.pressed()) {
				Difficulty = 3;
				init(Red, Blue, White, RedHado, BlueHado, RedHadoDisplay, BlueHadoDisplay);
				timer = 5400;
				timer2 = 240;
				Status = 13;
			}
		}
		else if (Status == 10) {
			RedGoal.draw({Color(255, 0, 0, 255), Color(255, 0, 0, 50), Color(255, 0, 0, 50), Color(255, 0, 0, 255)});
			BlueGoal.draw({Color(0, 0, 255, 50), Color(0, 0, 255, 255), Color(0, 0, 255, 255), Color(0, 0, 255, 50)});

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
			}
			RedHado = Min(RedHado, 240);


			if (Blue.circle.center.distanceFrom(White.circle.center) < BlueHado * 2 && Line(Blue.circle.center, White.circle.center + (White.circle.center - Blue.circle.center) * 1000).intersects(Line(100, 175, 100, 275))) {
				HadoArray.push_back({ Blue.circle.center, 0, BlueHado, false, true, false, Palette::Blue });
				BlueHado = 0;
			}
			else if (Difficulty == 3 && White.circle.center.distanceFrom(700, 225) < 200) {
				Vec2 Target(700, 225);
				if (Blue.circle.center.distanceFrom(Target) > 10) {
					Blue.velocity += (Target - Blue.circle.center) / (Target - Blue.circle.center).length() * 0.4;
					BlueHado++;
				}
				else if (Blue.circle.center.distanceFrom(White.circle.center) < BlueHado * 2) {
					HadoArray.push_back({ Blue.circle.center, 0, BlueHado, false, true, false, Palette::Blue });
					BlueHado = 0;
				}
				else {
					BlueHado++;
				}
			}
			else {
				Ball WhitePrediction;
				Vec2 BlueCenter, BlueVelocity;
				double DistanceReach, VelocityPrediction, Acceleration;

				WhitePrediction = White;
				BlueCenter = Blue.circle.center;
				BlueVelocity = Blue.velocity;
				DistanceReach = 0;
				VelocityPrediction = 0;

				if (Difficulty == 0) {
					Acceleration = 0.04;
				}
				else if (Difficulty == 1) {
					Acceleration = 0.1;
				}
				else if (Difficulty == 2) {
					Acceleration = 0.3;
				}
				else {
					Acceleration = 0.4;
				}
				for (int i = 0; i < 200; i++) {
					BlueVelocity *= 0.95;
					BlueCenter += BlueVelocity;
					VelocityPrediction += Acceleration;
					VelocityPrediction *= 0.95;
					DistanceReach += VelocityPrediction;
					WhitePrediction.move(FieldTopInvisible, FieldBottomInvisible, FieldLeftInvisible, FieldRightInvisible);
					Vec2 Target = WhitePrediction.circle.center + (WhitePrediction.circle.center - Vec2(100, 225)) / (WhitePrediction.circle.center - Vec2(100, 225)).length() * 20;
					if (BlueCenter.distanceFrom(Target) < DistanceReach) {
						Blue.velocity += (Target - Blue.circle.center) / (Target - Blue.circle.center).length() * Acceleration;
						break;
					}
				}
				BlueHado++;
			}
			BlueHado = Min(BlueHado, 240);

			RedHadoDisplay = (RedHado + RedHadoDisplay * 4) / 5;
			BlueHadoDisplay = (BlueHado + BlueHadoDisplay * 4) / 5;

			for (const Rect& r : FieldTop) {
				r.draw();
			}
			for (const Rect& r : FieldBottom) {
				r.draw();
			}
			for (const Rect& r : FieldLeft) {
				r.draw();
			}
			for (const Rect& r : FieldRight) {
				r.draw();
			}

			Red.cursor();
			Red.attenuate();
			Red.move(FieldTopInvisible, FieldBottomInvisible, FieldLeftInvisible, FieldRightInvisible);
			Blue.attenuate();
			Blue.move(FieldTopInvisible, FieldBottomInvisible, FieldLeftInvisible, FieldRightInvisible);
			White.attenuate();
			White.move(FieldTopInvisible, FieldBottomInvisible, FieldLeftInvisible, FieldRightInvisible);
			Red.draw();
			Blue.draw();
			White.draw();
			Red.collision(Blue);
			Red.collision(White);
			Blue.collision(White);

			fontMedium100(RedPoint).drawAt(200, 520, Palette::Red);
			fontMedium100(BluePoint).drawAt(600, 520, Palette::Blue);

			RedHadoMeter.drawFrame(0, 2, Color(255, 200, 200));
			Rect(120, 420, RedHadoDisplay, 40).draw(Palette::Red);
			BlueHadoMeter.drawFrame(0, 2, Color(200, 200, 255));
			Rect(680 - BlueHadoDisplay, 420, BlueHadoDisplay, 40).draw(Palette::Blue);
			fontRegular30(U"TIME").drawAt(400, 480);
			TimerBox.drawFrame(0, 3);
			fontMedium60(timer / 60).drawAt(TimerBox.center());
			if (Difficulty == 0) {
				fontRegular30(U"COM:よわい").draw(600, 0);
			}
			else if (Difficulty == 1) {
				fontRegular30(U"COM:ふつう").draw(600, 0);
			}
			else if (Difficulty == 2) {
				fontRegular30(U"COM:つよい").draw(600, 0);
			}
			else if (Difficulty == 3) {
				fontRegular30(U"COM:さいきょう").draw(550, 0);
			}
			timer--;
			if (timer == 0) {
				Status = 14;
			}
			else if (White.across(RedGoal)) {
				BluePoint++;
				RedHado = 0;
				BlueHado = 0;
				timer2 = 240;
				Status = 11;
			}
			else if (White.across(BlueGoal)) {
				RedPoint++;
				RedHado = 0;
				BlueHado = 0;
				timer2 = 240;
				Status = 12;
			}
		}
		else if (Status == 11) {
			fontMedium100(RedPoint).drawAt(200, 520, Palette::Red);
			fontMedium100(BluePoint).drawAt(600, 520, Palette::Blue);

			RedGoal.draw({ Color(255, 255, 0, 255), Color(255, 255, 0, 100), Color(255, 255, 0, 100), Color(255, 255, 0, 255) });
			BlueGoal.draw({ Color(0, 0, 255, 50), Color(0, 0, 255, 255), Color(0, 0, 255, 255), Color(0, 0, 255, 50) });

			for (const Rect& r : FieldTop) {
				r.draw();
			}
			for (const Rect& r : FieldBottom) {
				r.draw();
			}
			for (const Rect& r : FieldLeft) {
				r.draw();
			}
			for (const Rect& r : FieldRight) {
				r.draw();
			}
			Red.draw();
			Blue.draw();
			White.draw();			
			
			RedHadoDisplay = (RedHado + RedHadoDisplay * 4) / 5;
			BlueHadoDisplay = (BlueHado + BlueHadoDisplay * 4) / 5;

			RedHadoMeter.drawFrame(0, 2, Color(255, 200, 200));
			Rect(120, 420, RedHadoDisplay, 40).draw(Palette::Red);
			BlueHadoMeter.drawFrame(0, 2, Color(200, 200, 255));
			Rect(680 - BlueHadoDisplay, 420, BlueHadoDisplay, 40).draw(Palette::Blue);

			fontRegular30(U"TIME").drawAt(400, 480);
			TimerBox.drawFrame(0, 3);
			fontMedium60(timer / 60).drawAt(TimerBox.center());

			for (Hado& h : HadoArray) {
				h.draw();
				h.hit(Red, Blue, White);
				h.move();
			}
			while (!HadoArray.empty() && HadoArray.front().power <= 0) {
				HadoArray.pop_front();
			}
			Red.attenuate();
			Red.move(FieldTopInvisible, FieldBottomInvisible, FieldLeftInvisible, FieldRightInvisible);
			Blue.attenuate();
			Blue.move(FieldTopInvisible, FieldBottomInvisible, FieldLeftInvisible, FieldRightInvisible);
			White.attenuate();
			White.move(FieldTopInvisible, FieldBottomInvisible, FieldLeftInvisible, FieldRightInvisible);
			Red.draw();
			Blue.draw();
			White.draw();
			Red.collision(Blue);
			Red.collision(White);
			Blue.collision(White);
			if (Difficulty == 0) {
				fontRegular30(U"COM:よわい").draw(600, 0);
			}
			else if (Difficulty == 1) {
				fontRegular30(U"COM:ふつう").draw(600, 0);
			}
			else if (Difficulty == 2) {
				fontRegular30(U"COM:つよい").draw(600, 0);
			}
			else if (Difficulty == 3) {
				fontRegular30(U"COM:さいきょう").draw(550, 0);
			}
			fontHeavy255(U"BLUE GOAL!!!").drawAt(timer2 * 15 - 1400, 300, Palette::Yellow);
			timer2--;
			if (timer2 == 0) {
				init(Red, Blue, White, RedHado, BlueHado, RedHadoDisplay, BlueHadoDisplay);
				Status = 10;
			}
		}
		else if (Status == 12) {
			fontMedium100(RedPoint).drawAt(200, 520, Palette::Red);
			fontMedium100(BluePoint).drawAt(600, 520, Palette::Blue);

			RedGoal.draw({ Color(255, 0, 0, 255), Color(255, 0, 0, 50), Color(255, 0, 0, 50), Color(255, 0, 0, 255) });
			BlueGoal.draw({ Color(255, 255, 0, 100), Color(255, 255, 0, 255), Color(255, 255, 0, 255), Color(255, 255, 0, 100) });

			for (const Rect& r : FieldTop) {
				r.draw();
			}
			for (const Rect& r : FieldBottom) {
				r.draw();
			}
			for (const Rect& r : FieldLeft) {
				r.draw();
			}
			for (const Rect& r : FieldRight) {
				r.draw();
			}
			Red.draw();
			Blue.draw();
			White.draw();
	
			RedHadoDisplay = (RedHado + RedHadoDisplay * 4) / 5;
			BlueHadoDisplay = (BlueHado + BlueHadoDisplay * 4) / 5;
			
			RedHadoMeter.drawFrame(0, 2, Color(255, 200, 200));
			Rect(120, 420, RedHadoDisplay, 40).draw(Palette::Red);
			BlueHadoMeter.drawFrame(0, 2, Color(200, 200, 255));
			Rect(680 - BlueHadoDisplay, 420, BlueHadoDisplay, 40).draw(Palette::Blue);
			
			fontRegular30(U"TIME").drawAt(400, 480);
			TimerBox.drawFrame(0, 3);
			fontMedium60(timer / 60).drawAt(TimerBox.center());
			
			for (Hado& h : HadoArray) {
				h.draw();
				h.hit(Red, Blue, White);
				h.move();
			}
			while (!HadoArray.empty() && HadoArray.front().power <= 0) {
				HadoArray.pop_front();
			}
			Red.attenuate();
			Red.move(FieldTopInvisible, FieldBottomInvisible, FieldLeftInvisible, FieldRightInvisible);
			Blue.attenuate();
			Blue.move(FieldTopInvisible, FieldBottomInvisible, FieldLeftInvisible, FieldRightInvisible);
			White.attenuate();
			White.move(FieldTopInvisible, FieldBottomInvisible, FieldLeftInvisible, FieldRightInvisible);
			Red.draw();
			Blue.draw();
			White.draw();
			Red.collision(Blue);
			Red.collision(White);
			Blue.collision(White);
			if (Difficulty == 0) {
				fontRegular30(U"COM:よわい").draw(600, 0);
			}
			else if (Difficulty == 1) {
				fontRegular30(U"COM:ふつう").draw(600, 0);
			}
			else if (Difficulty == 2) {
				fontRegular30(U"COM:つよい").draw(600, 0);
			}
			else if (Difficulty == 3) {
				fontRegular30(U"COM:さいきょう").draw(550, 0);
			}
			fontHeavy255(U"RED GOAL!!!").drawAt(timer2 * 15 - 1400, 300, Palette::Yellow);
			timer2--;
			if (timer2 == 0) {
				init(Red, Blue, White, RedHado, BlueHado, RedHadoDisplay, BlueHadoDisplay);
				Status = 10;
			}
		}
		else if (Status == 13) {
			RedGoal.draw({ Color(255, 0, 0, 255), Color(255, 0, 0, 50), Color(255, 0, 0, 50), Color(255, 0, 0, 255) });
			BlueGoal.draw({ Color(0, 0, 255, 50), Color(0, 0, 255, 255), Color(0, 0, 255, 255), Color(0, 0, 255, 50) });
			for (const Rect& r : FieldTop) {
				r.draw();
			}
			for (const Rect& r : FieldBottom) {
				r.draw();
			}
			for (const Rect& r : FieldLeft) {
				r.draw();
			}
			for (const Rect& r : FieldRight) {
				r.draw();
			}
			Red.draw();
			Blue.draw();
			White.draw();
			RedHadoMeter.drawFrame(0, 2, Color(255, 200, 200));
			BlueHadoMeter.drawFrame(0, 2, Color(200, 200, 255));
			fontRegular30(U"TIME").drawAt(400, 480);
			TimerBox.drawFrame(0, 3);
			fontMedium60(timer / 60).drawAt(TimerBox.center());
			if (Difficulty == 0) {
				fontRegular30(U"COM:よわい").draw(600, 0);
			}
			else if (Difficulty == 1) {
				fontRegular30(U"COM:ふつう").draw(600, 0);
			}
			else if (Difficulty == 2) {
				fontRegular30(U"COM:つよい").draw(600, 0);
			}
			else if (Difficulty == 3) {
				fontRegular30(U"COM:さいきょう").draw(550, 0);
			}
			if (timer2 > 180) {
				Circle(400, 300, 150).drawArc(0, 1_pi * ((timer2 - 180) * 6) / 180, 0, 40, Palette::Yellow);
				fontMedium200(3).drawAt(400, 300);
			}
			else if (timer2 > 120) {
				Circle(400, 300, 150).drawArc(0, 1_pi * ((timer2 - 120) * 6) / 180, 0, 40, Palette::Orange);
				fontMedium200(2).drawAt(400, 300);
			}
			else if (timer2 > 60) {
				Circle(400, 300, 150).drawArc(0, 1_pi * ((timer2 - 60) * 6) / 180, 0, 40, Palette::Red);
				fontMedium200(1).drawAt(400, 300);
			}
			else {
				fontHeavy255(U"START!!").drawAt(timer2 * 160 / 6 - 800, 300, Palette::Yellow);
			}
			timer2--;
			if (timer2 == 0) {
				Status = 10;
			}
		}
		else if (Status == 14) {
			Rect(360, 338, 80, 23).draw();
			Rect(20, 225, 760, 250).drawFrame(0, 8);
			fontRegular255(RedPoint).drawAt(200, 350, Palette::Red);
			fontRegular255(BluePoint).drawAt(600, 350, Color(50, 50, 255));
			if (RedPoint > BluePoint) {
				fontMedium150(U"YOU WIN!").drawAt(400, 100, Palette::Red);
				if (Difficulty == 2) {
					fontRegular20(U"難易度選択画面でT,M,J,Nを同時押しすると...?").drawAt(400, 200);
				}
				else if (Difficulty == 3) {
					fontRegular20(U"「さいきょう」に勝利! Congratulation!!").drawAt(400, 200);
				}
			}
			else if (RedPoint < BluePoint) {
				fontMedium150(U"YOU LOSE").drawAt(400, 100, Color(50, 50, 255));
			}
			else {
				fontMedium150(U"DRAW").drawAt(400, 100);
			}
			fontRegular30(U"Thank you for playing!").drawAt(400, 550);
			fontRegular20(U"エンターキーでおわる").drawAt(400, 650);
			if (KeyEnter.down()) {
				return;
			}
		}
	}
}