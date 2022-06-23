#pragma once
#include <d3d9.h>
#include <d3dx9.h>

extern const int  winWidth, winHeight, SPTWIDHT, SPTHEIGHT;

struct Sprite
{
		RECT srcRect, destRect;
		IDirect3DSurface9* surface = nullptr;

		void Move(const int& x, const int& y) 
		{
			if(x + SPTWIDHT <= winWidth)
				destRect.right  = (destRect.left = x) + SPTWIDHT;
			if (y + SPTHEIGHT <= winHeight)
				destRect.bottom = (destRect.top = y) + SPTHEIGHT;
		}

		void MoveAdd(const int& x, const int& y)
		{
			if (destRect.right + x > winWidth)
				directionX = -1;
			else if (destRect.left - x < 0)
				directionX = 1;
			if (destRect.bottom + y > winHeight)
				directionY = -1;
			else if (destRect.top - y < 0)
				directionY = 1;

		destRect.right = (destRect.left += x * directionX) + SPTWIDHT;
		destRect.bottom = (destRect.top += y * directionY) + SPTHEIGHT;
		}

private:
	int directionX =1, directionY = 1;
};

