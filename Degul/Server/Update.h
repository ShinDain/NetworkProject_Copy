#pragma once
#include "global.h"

void Makeitem(float eTime)
{
	// 아이템 생성
	if (genItemTime>3)
	{
		for (int i = 0; i < MAX_ITEM_COUNT; i++)
		{
			if (!itemCreated[i])
			{
				itemCreated[i] = true;
				int rx, ry;
				while (true)
				{
					rx = ARENA_PIVOT_X + (rand() % (ARENA_RADIUS * 2) - ARENA_RADIUS);
					ry = ARENA_PIVOT_Y + (rand() % (ARENA_RADIUS * 2) - ARENA_RADIUS);
					if (ARENA_RADIUS * ARENA_RADIUS - 10 > (ARENA_PIVOT_X - rx) * (ARENA_PIVOT_X - rx) + (ARENA_PIVOT_Y - ry) * (ARENA_PIVOT_Y - ry))break;
				}
				items[i].x = rx;
				items[i].y = ry;

				int setTpye = rand() % 3;
				if (setTpye == 0)items[i].itemType = weight;
				if (setTpye == 1)items[i].itemType = lightFeather;
				if (setTpye == 2)items[i].itemType = powerUp;

				break;
			}
		}
		genItemTime = 0;
	}
	else
	{
		genItemTime += eTime;
	}
}
void CollisionItem(float eTime)
{
	for (int i = 0; i < MAX_CLIENT_COUNT; i++)
	{
		if (!threadIndexCheck[i])continue;
		for (int j = 0; j < MAX_ITEM_COUNT; j++)
		{
			if (itemCreated[j])
			{
				if (4* BALL_RADIUS * BALL_RADIUS > (BallData[i].x - items[j].x) * (BallData[i].x - items[j].x) + (BallData[i].y - items[j].y) * (BallData[i].y - items[j].y))
				{
					itemCreated[j] = false;
					if (items[i].itemType == weight)BallData[i].mass *= 2;
					else if (items[i].itemType == lightFeather)BallData[i].mass *= 0.5f;
					else if (items[i].itemType == powerUp)BallData[i].speedRate += 0.1;

					if (BallData[i].mass >= 4)BallData[i].mass = 4;
					if (BallData[i].mass <= 1)BallData[i].mass = 1;
				}
			}
		}
	}
}

void AddForce(BallInfo& b, float forceX, float forceY, float eTime)
{
	float SPEED = 5000.f;
	float accX = forceX / b.mass * SPEED;
	float accY = forceY / b.mass * SPEED;

	b.speedx += accX * eTime;
	b.speedy += accY * eTime;
	float SPEED_LIMIT = 200.0f;
	if (b.speedx > SPEED_LIMIT) b.speedx = SPEED_LIMIT;
	if (b.speedx < -SPEED_LIMIT) b.speedx = -SPEED_LIMIT;
	if (b.speedy > SPEED_LIMIT) b.speedy = SPEED_LIMIT;
	if (b.speedy < -SPEED_LIMIT) b.speedy = -SPEED_LIMIT;
}

void ModifyBallPos(BallInfo& b1, BallInfo& b2)
{
	float e = 0.9f;
	float speedx1 = ((b1.mass - b2.mass * e) / (b1.mass + b2.mass)) * b1.speedx + ((b2.mass + b2.mass * e) / (b1.mass + b2.mass)) * b2.speedx;
	float speedy1 = ((b1.mass - b2.mass * e) / (b1.mass + b2.mass)) * b1.speedy + ((b2.mass + b2.mass * e) / (b1.mass + b2.mass)) * b2.speedy;
	float speedx2 = ((b2.mass + b1.mass * e) / (b1.mass + b2.mass)) * b1.speedx + ((b2.mass - b1.mass * e) / (b1.mass + b2.mass)) * b2.speedx;
	float speedy2 = ((b2.mass + b1.mass * e) / (b1.mass + b2.mass)) * b1.speedy + ((b2.mass - b1.mass * e) / (b1.mass + b2.mass)) * b2.speedy;

	b1.speedx = speedx1 * b1.speedRate;
	b1.speedy = speedy1 * b1.speedRate;
	b2.speedx = speedx2 * b2.speedRate;
	b2.speedy = speedy2 * b2.speedRate;

	//겹침 제거
	float directX = b1.x - b2.x;
	float directY = b1.y - b2.y;
	float distance = sqrtf(directX * directX + directY * directY);
	if (distance > FLT_EPSILON) {
		directX /= distance;
		directY /= distance;
	}
	else {
		directX = 1;
		directY = 0;
	}
	if (distance < 2 * BALL_RADIUS) {
		float distanceToMove = 2 * BALL_RADIUS - distance;
		b1.x += directX * distanceToMove;
		b1.y += directY * distanceToMove;
		b2.x -= directX * distanceToMove;
		b2.y -= directY * distanceToMove;
	}
}

bool PlayerInArena(BallInfo ball)
{
	if (ball.x > ARENA_PIVOT_X + ARENA_RADIUS)
	{
		return false;
	}
	if (ball.y > ARENA_PIVOT_Y + ARENA_RADIUS)
	{
		return false;
	}
	if (ball.x < ARENA_PIVOT_X - ARENA_RADIUS)
	{
		return false;
	}
	if (ball.y < ARENA_PIVOT_Y - ARENA_RADIUS)
	{
		return false;
	}

	if (ARENA_RADIUS * ARENA_RADIUS < (ARENA_PIVOT_X - ball.x) * (ARENA_PIVOT_X - ball.x) + (ARENA_PIVOT_Y - ball.y) * (ARENA_PIVOT_Y - ball.y))
	{
		return false;
	}

	return true;
}

void MakeKillLog(float eTime, BallInfo b1, BallInfo b2,int kIndex)
{
	if (b2.mass == 0)return;
	KillLog kl;
	memcpy(kl.ID_Victim, b1.id, sizeof(b1.id));
	memcpy(kl.ID_Killer, b2.id, sizeof(b2.id));

	KillLog_LifeTime kll;
	kll.killLog = kl;
	kll.lifeTime = 0;
	killogs.push_back(kll);

	//점수 처리
	if (kIndex >= 0)playScoreboard.score[kIndex]++;
}

void CollisionBall(float eTime)
{
	for (int i = 0; i < MAX_CLIENT_COUNT - 1; i++)
	{
		if (!threadIndexCheck[i] || !BallData[i].isAlive) continue;
		for (int j = i + 1; j < MAX_CLIENT_COUNT; j++)
		{
			if (!BallData[j].isAlive) continue;
			if (4.f * BALL_RADIUS * BALL_RADIUS > (BallData[i].x - BallData[j].x) * (BallData[i].x - BallData[j].x) + (BallData[i].y - BallData[j].y) * (BallData[i].y - BallData[j].y))
			{
				ModifyBallPos(BallData[i], BallData[j]);
				lastCollision[i] = BallData[j];
				lastCollision[j] = BallData[i];
				lastCollisionIndex[i] = j;
				lastCollisionIndex[j] = i;

				/*if (!PlayerInArena(BallData[i]))
				{
					MakeKillLog(eTime, BallData[j], BallData[i]);
					BallData[i].isAlive = false;
					rebirthTime[i] = 0;
					playScoreboard.score[i]++;
				}

				if (!PlayerInArena(BallData[j]))
				{
					MakeKillLog(eTime, BallData[i], BallData[j]);
					BallData[j].isAlive = false;
					rebirthTime[j] = 0;
					playScoreboard.score[j]++;
				}*/
			}
		}
	}
}

void UpdateBall(float eTime)
{
	for (int i = 0; i < MAX_CLIENT_COUNT; i++)
	{
		if (!bPlayerIn[i])
			continue;
		if (!BallData[i].isAlive)
		{
			if (rebirthTime[i] > 1)
			{
				BallData[i].isAlive = true;

				int r = rand() % 5;
				BallData[i].x = InitPlayerPos_x[r];
				BallData[i].y = InitPlayerPos_y[r];
				BallData[i].speedx = 0;
				BallData[i].speedy = 0;

				BallInfo temp;
				temp.mass = 0;
				lastCollision[i] = temp;
				lastCollisionIndex[i] = -1;
			}
			else
			{
				rebirthTime[i]+=eTime;
			}
		}
		if (BallData[i].isAlive)
		{
			AddForce(BallData[i], Input[i].speedx, Input[i].speedy, eTime);
			
			float fric = 60.f;

			float mag = sqrt(BallData[i].speedx * BallData[i].speedx + BallData[i].speedy * BallData[i].speedy);
			if (mag > FLT_EPSILON) {
				float velDirX = -BallData[i].speedx / mag;
				float velDirY = -BallData[i].speedy / mag;
				velDirX *= fric;
				velDirY *= fric;
				velDirX /= BallData[i].mass;
				velDirY /= BallData[i].mass;
				BallData[i].speedx += velDirX;
				BallData[i].speedy += velDirY;

				//마찰에 의해 속도가 반대로 되지 않게함
				if (velDirX * BallData[i].speedx > 0) BallData[i].speedx = 0;
				if (velDirY * BallData[i].speedy > 0) BallData[i].speedy = 0;
			}
			BallData[i].x += BallData[i].speedx * eTime;
			BallData[i].y += BallData[i].speedy * eTime;
			//printf("Speed X: %f, Y: %f\n", BallData[i].speedx, BallData[i].speedy);

			//BallData[i].x += BallData[i].speedx;
			//BallData[i].y += BallData[i].speedy;
			if (!PlayerInArena(BallData[i]) && bMainGame)
			{
				BallData[i].isAlive = false;
				rebirthTime[i] = 0;

				MakeKillLog(eTime, BallData[i], lastCollision[i], lastCollisionIndex[i]);
			}
			else if (!PlayerInArena(BallData[i]) && !bMainGame)
			{
				BallData[i].x = ARENA_PIVOT_X;
				BallData[i].y = ARENA_PIVOT_Y;
				BallData[i].speedx = 0;
				BallData[i].speedy = 0;
			}
		}
	}
}

void Update(float eTime)
{
	// 상태별로 조건 넣기
	UpdateBall(eTime);
	CollisionItem(eTime);
	CollisionBall(eTime);
	Makeitem(eTime);

	//UpdatedDataToBuff();
}