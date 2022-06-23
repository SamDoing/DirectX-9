#ifndef UNICODE
#define UNICODE
#endif // !UNICODE

#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <vector>
#include <string>
#include "Sprite.h"

//#define SPRITE
//#define COLOR_MODE

#ifdef COLOR_MODE
#define D3DFVF_CUSTOMVERTEX  D3DFVF_XYZ | D3DFVF_DIFFUSE
#else
#define D3DFVF_CUSTOMVERTEX  D3DFVF_XYZ | D3DFVF_TEX1
#endif 


const int SPTWIDHT = 400, SPTHEIGHT = 400;
const int winWidth = 1280, winHeight = 720;

HINSTANCE hInst;
HWND wndHandle;

LPDIRECT3D9 pD3D;
LPDIRECT3DDEVICE9 pd3dDevice;

LPDIRECT3DVERTEXBUFFER9 vBuffer = NULL;
LPDIRECT3DINDEXBUFFER9 iBuffer = NULL;

std::vector<std::string> adapterDetais;
std::vector<Sprite> sprites;
std::vector<LPD3DXMESH> meshes;

#ifdef COLOR_MODE
struct CUSTOMVERTEX { float x, y, z; DWORD color; };
#else
struct CUSTOMVERTEX { float x, y, z, u, v; };
#endif


#pragma region FuncDeclarations

bool initWindow(const HINSTANCE& hInstance);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
bool initDirect3D();
void render();
void myGetAdpaterInformation();
const bool initSprites( void );
IDirect3DSurface9* GetSurfaceFromBitmap( const char* );
HRESULT SetupVB();
HRESULT SetupMesh();

#pragma endregion


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PWSTR lpCmdLine, int nCmdShow)
{
	if ( !initWindow(hInstance) )
		return FALSE;

	if ( !initDirect3D() ) {
		return FALSE;
	}

#ifdef SPRITE
	if ( !initSprites() ) {
		return false;
	}
#endif

	if ( FAILED( SetupVB() ) || FAILED( SetupMesh() ) )
		return FALSE;

	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);

		}
		else
		{
			render();
		}
	}

	//Releasing device and the Direct3D object
	if (pd3dDevice != NULL)
		pd3dDevice->Release();
	if (pD3D != NULL)
		pD3D->Release();

	return (int)msg.wParam;
}


bool initWindow(const HINSTANCE& hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = 0;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"DirectXExample";
	wcex.hIconSm = 0;

	RegisterClassEx(&wcex);

	wndHandle = CreateWindow(
		L"DirectXExample",
		L"DirectXExample title",
		WS_EX_TOPMOST | WS_POPUP | WS_VISIBLE,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		winWidth,
		winHeight,
		NULL,
		NULL,
		hInstance,
		NULL);

	if (!wndHandle)
	{
		return false;
	}

	ShowWindow(wndHandle, SW_SHOW);
	UpdateWindow(wndHandle);
	return true;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}


#define FLDHR(hR) if ( FAILED( hR ) ) { return NULL; }
HRESULT SetupVB()
{
	constexpr float textureTimes = 1.0f;

	CUSTOMVERTEX vertices[] =
	{
#ifdef COLOR_MODE
		//CUBE without index buffer
		//1
		{ -1.0f,  1.0f, -1.0f, D3DCOLOR_ARGB(0,0,0,255) },
		{  1.0f,  1.0f, -1.0f, D3DCOLOR_ARGB(0,0,0,255) },
		{ -1.0f, -1.0f, -1.0f, D3DCOLOR_ARGB(0,0,0,255) },
		{  1.0f, -1.0f, -1.0f, D3DCOLOR_ARGB(0,0,0,255) },
		//2						
		{ -1.0f,  1.0f, 1.0f, D3DCOLOR_ARGB(0,0,255,0) },
		{ -1.0f, -1.0f, 1.0f, D3DCOLOR_ARGB(0,0,255,0) },
		{  1.0f,  1.0f, 1.0f, D3DCOLOR_ARGB(0,0,255,0) },
		{  1.0f, -1.0f, 1.0f, D3DCOLOR_ARGB(0,0,255,0) },
		//3						
		{ -1.0f, 1.0f,  1.0f, D3DCOLOR_ARGB(0,255,0,0) },
		{  1.0f, 1.0f,  1.0f, D3DCOLOR_ARGB(0,255,0,0) },
		{ -1.0f, 1.0f, -1.0f, D3DCOLOR_ARGB(0,255,0,0) },
		{  1.0f, 1.0f, -1.0f, D3DCOLOR_ARGB(0,255,0,0) },
		//4						
		{ -1.0f, -1.0f,  1.0f, D3DCOLOR_ARGB(0,127,0,127) },
		{ -1.0f, -1.0f, -1.0f, D3DCOLOR_ARGB(0,127,0,127) },
		{  1.0f, -1.0f,  1.0f, D3DCOLOR_ARGB(0,127,0,127) },
		{  1.0f, -1.0f, -1.0f, D3DCOLOR_ARGB(0,127,0,127) },
		//5						
		{  1.0f,  1.0f, -1.0f, D3DCOLOR_ARGB(0,0,0,255) },
		{  1.0f,  1.0f,  1.0f, D3DCOLOR_ARGB(0,0,0,255) },
		{  1.0f, -1.0f, -1.0f, D3DCOLOR_ARGB(0,0,0,255) },
		{  1.0f, -1.0f,  1.0f, D3DCOLOR_ARGB(0,0,0,255) },
		//6						
		{ -1.0f,  1.0f, -1.0f, D3DCOLOR_ARGB(0,0,0,255) },
		{ -1.0f, -1.0f, -1.0f, D3DCOLOR_ARGB(0,0,0,255) },
		{ -1.0f,  1.0f,  1.0f, D3DCOLOR_ARGB(0,0,0,255) },
		{ -1.0f, -1.0f,  1.0f, D3DCOLOR_ARGB(0,0,0,255) }
		
		/*
		//CUBE with index buffer
		{ -1.0f,  -1.0f, -1.0f, D3DCOLOR_ARGB(0,0,0,255) },
		{ -1.0f,   1.0f, -1.0f, D3DCOLOR_ARGB(0,0,0,255) },
		{  1.0f,   1.0f, -1.0f, D3DCOLOR_ARGB(0,0,0,255) },
		{  1.0f,  -1.0f, -1.0f, D3DCOLOR_ARGB(0,0,0,255) },

		{ -1.0f,  -1.0f,  1.0f, D3DCOLOR_ARGB(0,0,0,255) },
		{  1.0f,  -1.0f,  1.0f, D3DCOLOR_ARGB(0,0,0,255) },
		{  1.0f,   1.0f,  1.0f, D3DCOLOR_ARGB(0,0,0,255) },
		{ -1.0f,   1.0f,  1.0f, D3DCOLOR_ARGB(0,0,0,255) }
		*/
#else
		//CUBE without index buffer
	//1
	{ -1.0f,  1.0f, -1.0f, 0.0f, 0.0f },
	{  1.0f,  1.0f, -1.0f, textureTimes, 0.0f },
	{ -1.0f, -1.0f, -1.0f, 0.0f, textureTimes },
	{  1.0f, -1.0f, -1.0f, textureTimes, textureTimes },
	//2						
	{ -1.0f,  1.0f, 1.0f,  0.0f, 0.0f },
	{ -1.0f, -1.0f, 1.0f,  textureTimes, 0.0f },
	{  1.0f,  1.0f, 1.0f,  0.0f, textureTimes },
	{  1.0f, -1.0f, 1.0f,  textureTimes, textureTimes },
	//3						
	{ -1.0f, 1.0f,  1.0f, 0.0f, 0.0f },
	{  1.0f, 1.0f,  1.0f, textureTimes, 0.0f },
	{ -1.0f, 1.0f, -1.0f, 0.0f, textureTimes },
	{  1.0f, 1.0f, -1.0f, textureTimes, textureTimes },
	//4						
	{ -1.0f, -1.0f,  1.0f, 0.0f, 0.0f },
	{ -1.0f, -1.0f, -1.0f, textureTimes, 0.0f },
	{  1.0f, -1.0f,  1.0f, 0.0f, textureTimes },
	{  1.0f, -1.0f, -1.0f, textureTimes, textureTimes },
	//5						
	{  1.0f,  1.0f, -1.0f,  0.0f, 0.0f },
	{  1.0f,  1.0f,  1.0f,  textureTimes, 0.0f },
	{  1.0f, -1.0f, -1.0f,  0.0f, textureTimes },
	{  1.0f, -1.0f,  1.0f,  textureTimes, textureTimes },
	//6						
	{ -1.0f,  1.0f, -1.0f,  0.0f, 0.0f },
	{ -1.0f, -1.0f, -1.0f,  textureTimes, 0.0f },
	{ -1.0f,  1.0f,  1.0f,  0.0f, textureTimes },
	{ -1.0f, -1.0f,  1.0f,  textureTimes, textureTimes }

	/*
	//CUBE with index buffer
	{ -1.0f,   1.0f, -1.0f, 1.0f, 0.0f },
	{  1.0f,   1.0f, -1.0f, 0.0f, 1.0f },
	{  1.0f,  -1.0f, -1.0f, 1.0f, 1.0f },
	{ -1.0f,  -1.0f, -1.0f, 0.0f, 0.0f },

	{ -1.0f,  -1.0f,  1.0f, 0.0f, 0.0f },
	{  1.0f,  -1.0f,  1.0f, 1.0f, 0.0f },
	{  1.0f,   1.0f,  1.0f, 0.0f, 1.0f },
	{ -1.0f,   1.0f,  1.0f, 1.0f, 1.0f }
	*/
#endif 
	};

	WORD indexData[]
	{
		0,1,2,		//triangle 1
		2,3,0,		//triangle 2
		4,5,6,		//triangle 3
		6,7,4,		//triangle 4
		0,3,5,		//triangle 5
		5,4,0,		//triangle 6
		3,2,6,		//triangle 7
		6,5,3,		//triangle 8
		2,1,7,		//triangle 9
		7,6,2,		//triangle 10
		1,0,4,		//triangle 11
		4,7,1		//triangle 12

	};


	HRESULT hR;


	hR = pd3dDevice->CreateIndexBuffer( 
		sizeof(indexData), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &iBuffer, NULL );

	FLDHR(hR);

	hR = pd3dDevice->CreateVertexBuffer(
		sizeof(vertices), 0, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &vBuffer, NULL);

	FLDHR(hR);
	
	//Index Buffer
	void* pIVertices = NULL;
	hR = iBuffer->Lock(0, 0, &pIVertices, D3DLOCK_DISCARD);

	FLDHR(hR);

	memcpy( pIVertices, indexData, sizeof(indexData) );

	hR = iBuffer->Unlock();
	FLDHR(hR);

	//Vertex Buffer
	void* pVertices = NULL;
	hR = vBuffer->Lock(0, sizeof(vertices), &pVertices, 0);

	FLDHR(hR);

	memcpy(pVertices, vertices, sizeof(vertices));

	hR = vBuffer->Unlock();
	FLDHR(hR);
#ifndef COLOR_MODE
	LPDIRECT3DTEXTURE9 pTexture = NULL;
	hR = D3DXCreateTextureFromFileA( pd3dDevice, "Sprit.bmp", &pTexture );
	FLDHR(hR);

	pd3dDevice->SetTexture( 0, pTexture );
#endif // !COLOR_MODE


	return S_OK;
} 


HRESULT SetupMesh() 
{
	//Create vertex and info for mesh
	CUSTOMVERTEX vertices[] =
	{
		//CUBE with index buffer
		{ -1.0f,  -1.0f, -1.0f, D3DCOLOR_ARGB(0,0,0,255) },
		{ -1.0f,   1.0f, -1.0f, D3DCOLOR_ARGB(0,0,0,255) },
		{  1.0f,   1.0f, -1.0f, D3DCOLOR_ARGB(0,0,0,255) },
		{  1.0f,  -1.0f, -1.0f, D3DCOLOR_ARGB(0,0,0,255) },

		{ -1.0f,  -1.0f,  1.0f, D3DCOLOR_ARGB(0,0,0,255) },
		{  1.0f,  -1.0f,  1.0f, D3DCOLOR_ARGB(0,0,0,255) },
		{  1.0f,   1.0f,  1.0f, D3DCOLOR_ARGB(0,0,0,255) },
		{ -1.0f,   1.0f,  1.0f, D3DCOLOR_ARGB(0,0,0,255) }
	};

	WORD indexData[]
	{
		0,1,2,		//triangle 1
		2,3,0,		//triangle 2
		4,5,6,		//triangle 3
		6,7,4,		//triangle 4
		0,3,5,		//triangle 5
		5,4,0,		//triangle 6
		3,2,6,		//triangle 7
		6,5,3,		//triangle 8
		2,1,7,		//triangle 9
		7,6,2,		//triangle 10
		1,0,4,		//triangle 11
		4,7,1		//triangle 12

	};

	
	//Create mesh
	HRESULT hR;

	LPD3DXMESH boxMesh;

	D3DVERTEXELEMENT9 Declaration[MAX_FVF_DECL_SIZE];

	D3DXDeclaratorFromFVF(D3DFVF_CUSTOMVERTEX, Declaration);

	hR = D3DXCreateMesh(12, 8, D3DXMESH_MANAGED, Declaration, pd3dDevice, &boxMesh);

	FLDHR(hR);

	VOID* pVertices;
	VOID* pIndex;

	hR = boxMesh->LockVertexBuffer( D3DLOCK_DISCARD, &pVertices );
	FLDHR(hR);
	
	hR = boxMesh->LockIndexBuffer( 0, &pIndex );
	FLDHR(hR);

	memcpy( pVertices, vertices, sizeof( vertices ) );
	memcpy( pIndex, indexData, sizeof( indexData ) );

	boxMesh->UnlockVertexBuffer();
	boxMesh->UnlockIndexBuffer();

	//Optimize mesh
	boxMesh->OptimizeInplace( D3DXMESHOPT_ATTRSORT, 0, NULL, NULL, NULL);

	meshes.push_back( boxMesh );

}
#undef FLDHR


bool initDirect3D(void)
{
	pD3D = NULL;
	pd3dDevice = NULL;

	if(NULL == ( pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
	{
		return false;
	}

	//myGetAdpaterInformation(); // -> Get adapter information like name, description, modes...


	D3DPRESENT_PARAMETERS d3dpp;

	ZeroMemory( &d3dpp, sizeof( d3dpp ) );
	d3dpp.Windowed = FALSE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferCount = 1;
	d3dpp.BackBufferHeight = winHeight;
	d3dpp.BackBufferWidth = winWidth;
	d3dpp.hDeviceWindow = wndHandle;

	if(FAILED (pD3D->CreateDevice( D3DADAPTER_DEFAULT,
								   D3DDEVTYPE_REF,
							       wndHandle,
							       D3DCREATE_SOFTWARE_VERTEXPROCESSING,
								   &d3dpp,
								   &pd3dDevice ) ) )
	{ return false; }

	return true;
}


float countR = 0.0f;
void render() 
{  
#ifdef SPRITE
	auto drawSprites = [&]() 
	{
		//Get that back buffer surface
		IDirect3DSurface9* backBuffer;
		if (SUCCEEDED(
			pd3dDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backBuffer)))
		{
			//Put that surface on our back buffer surface
			for (Sprite& sprite : sprites)
			{
				pd3dDevice->StretchRect(sprite.surface, NULL,
					backBuffer, &sprite.destRect, D3DTEXF_NONE);

				sprite.MoveAdd(1, 1);
			}

		}
	};
#endif
	if (pd3dDevice == NULL)
		return;

	pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET,
						D3DCOLOR_XRGB( 255,255,255 ), 1.0f, 0 );

#ifdef SPRITE
		drawSprites();
#endif


	D3DXMATRIX  matView, matIde, matProjection, matRot, endMat,middleMat;

	countR = countR >= 360.0f ? 0.0f:(countR+0.1f);
	pd3dDevice->BeginScene();
	{
		pd3dDevice->SetStreamSource( 0, vBuffer, 0, sizeof(CUSTOMVERTEX) );
		pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
		pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
		pd3dDevice->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_ARGB( 255,255,255,255 ) );

		
		D3DXMatrixIdentity(&matIde);
		D3DXMatrixRotationY(&matRot, countR);
		D3DXMatrixRotationX(&matIde, countR );
		D3DXMatrixMultiply(&endMat, &matIde, &matRot);
		//D3DXMatrixScaling(&middleMat, countR/180.0f, countR / 180.0f, countR / 180.0f);
		//D3DXMatrixMultiply(&endMat, &endMat, &middleMat);
		pd3dDevice->SetTransform(D3DTS_WORLD, &endMat);

		
		D3DXMatrixLookAtLH(&matView,
			&D3DXVECTOR3(0.0f, 0.0f, 10.0f),    //the camera position
			&D3DXVECTOR3(0.0f, 0.0f, 0.0f),     //the look-at position
			&D3DXVECTOR3(0.0f, 1.0f, 0.0f));    //the up direction
		pd3dDevice->SetTransform(D3DTS_VIEW, &matView);

		
		D3DXMatrixPerspectiveFovLH(&matProjection,
			D3DXToRadian(45),							// the horizontal field of view
			(FLOAT)winWidth / (FLOAT)winHeight,			// aspect ratio
			1.0f,										// the near view-plane
			100.0f);									// the far view-plane
		pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProjection);


		//pd3dDevice->SetIndices( iBuffer );
		//pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 4);
		for( LPD3DXMESH& mesh : meshes )
			mesh->DrawSubset(0);

		//for (size_t i = 0; i < 6; i++)
			//pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 4 * i, 2 );
	}
	pd3dDevice->EndScene();

	pd3dDevice->Present( NULL, NULL, NULL, NULL );
}


//Adapter information gathering
void myGetAdpaterInformation() 
{
	D3DADAPTER_IDENTIFIER9 ident;
	pD3D->GetAdapterIdentifier(D3DADAPTER_DEFAULT, 0, &ident);

	adapterDetais.reserve(5);
	adapterDetais.emplace_back(ident.Description);
	adapterDetais.emplace_back(ident.DeviceName);
	adapterDetais.emplace_back(ident.Driver);

	adapterDetais.emplace_back("Adapter modes");

	UINT numModes = pD3D->GetAdapterModeCount( D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8 );
	for (size_t i = 0; i < numModes; i++)
	{
		D3DDISPLAYMODE mode;
		char modeString[255];

		pD3D->EnumAdapterModes(D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8, i, &mode);

		sprintf_s(modeString, "Width=%d Height=%d Refresh Rate =%d",
			mode.Width, mode.Height, mode.RefreshRate);
		adapterDetais.emplace_back(modeString);
	}
}


const bool initSprites(void) 
{
	for (size_t i = 0; i < 10; i++)
		sprites.emplace_back();

	int posM = 1;
	for (Sprite& sprite : sprites)
	{
		sprite.destRect.left = rand() % 640 - SPTWIDHT;
		sprite.destRect.top = rand() % 480 - SPTHEIGHT;
		sprite.destRect.right = sprite.destRect.left + SPTWIDHT;
		sprite.destRect.bottom = sprite.destRect.top + SPTHEIGHT;

		sprite.surface = GetSurfaceFromBitmap("Sprit.bmp");
	}

	for (const Sprite& sprite : sprites)
		if (sprite.surface == nullptr)
			return false;
	return true;
}

void CreateLight(const D3DLIGHTTYPE& lType) 
{
	D3DLIGHT9 light;
	light.Type = lType;
	
	switch (lType) 
	{
	case	D3DLIGHT_DIRECTIONAL:
		D3DXVec3Normalize((D3DXVECTOR3*)&light.Direction, &D3DXVECTOR3(1.0f, 0.0f, 0.0f));

		light.Diffuse.r = 0.0f;
		light.Diffuse.g = 0.0f;
		light.Diffuse.b = 0.5f;

		light.Ambient.r = 0.0f;
		light.Ambient.g = 0.0f;
		light.Ambient.b = 0.3f;
		break;

	case D3DLIGHT_POINT:
		light.Position = D3DXVECTOR3(-2500.0f, 0.0f, 0.0f);

		light.Diffuse.r = 1.0f;
		light.Diffuse.g = 0.5f;
		light.Diffuse.b = 0.5f;

		light.Ambient.r = 0.5f;
		light.Ambient.g = 0.0f;
		light.Ambient.b = 0.0f;
		break;

	case D3DLIGHT_SPOT:
		D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &D3DXVECTOR3( 1.0f, -1.0f, 0.0f ) );
		light.Position = D3DXVECTOR3( -250.0f, 250.0f, 0.0f );
		
		light.Diffuse.r = 1.0f;
		light.Diffuse.g = 0.5f;
		light.Diffuse.b = 0.5f;

		light.Ambient.r = 0.5f;
		light.Ambient.g = 0.0f;
		light.Ambient.b = 0.0f;

		light.Phi = 1.0f;
		light.Falloff = 0.5f;
		light.Theta = 0.5f;
		break;

	default:
		return;
	}
	
	light.Range = sqrtf(FLT_MAX);
	
	pd3dDevice->SetLight( 0, &light );
	pd3dDevice->LightEnable( 0, TRUE );
}

IDirect3DSurface9* GetSurfaceFromBitmap(const char* fileName)
{
	IDirect3DSurface9* surface = nullptr;
	D3DXIMAGE_INFO imageInfo;
	HRESULT hResult = D3DXGetImageInfoFromFileA(fileName, &imageInfo);
	
	if ( SUCCEEDED ( hResult ) )
	{
		hResult = pd3dDevice->CreateOffscreenPlainSurface(imageInfo.Width,
			imageInfo.Height, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &surface, NULL);
		
		if ( SUCCEEDED( hResult ) )
			hResult = D3DXLoadSurfaceFromFileA(surface, NULL, NULL, fileName, NULL, D3DX_DEFAULT, 0, NULL);
		
	}

	return SUCCEEDED( hResult ) ? surface : nullptr;
}