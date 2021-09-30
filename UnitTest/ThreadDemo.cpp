#include "stdafx.h"
#include "ThreadDemo.h"

void ThreadDemo::Initialize()
{
	//Loop();

	//MultiThread();
	//Join();

	//Mutex();

	//ExecTimer();
	ExecPerfomence();
}

void ThreadDemo::Update()
{
	m.lock();
	{
		progress += 0.1f;

		ImGui::ProgressBar(progress / 1000.0f);
	}
	m.unlock();
}

void ThreadDemo::Loop()
{
	for (int i = 0; i < 100; i++)
		printf("1 : %d\n", i);
	printf("반복문1 종료\n");

	for (int i = 0; i < 100; i++)
		printf("2 : %d\n", i);
	printf("반복문2 종료\n");
}

void ThreadDemo::MultiThread()
{
	thread t(bind(&ThreadDemo::MultiThread1, this));
	thread t2(bind(&ThreadDemo::MultiThread2, this));

	t2.join();

	printf("t2.join\n\n");

	t.join();

	printf("t.join\n\n");
}

void ThreadDemo::MultiThread1()
{
	for (int i = 0; i < 100; i++)
		printf("1 : %d\n", i);
	printf("반복문1 종료\n");
}

void ThreadDemo::MultiThread2()
{
	for (int i = 0; i < 100; i++)
		printf("2 : %d\n", i);
	printf("반복문2 종료\n");
}

void ThreadDemo::Join()
{
	thread t([]()
	{
		for (int i = 0; i < 1000; i++)
			printf("1 : %d\n", i);

		printf("반복문1 종료\n");
	});

	thread t2([]() 
	{
		int a = 0;
		while (true)
		{
			a++;
			printf("A : %d\n", a);

			Sleep(100);

			if (a > 30)
				break;
		}
	});

	printf("멀티 쓰레드 시작");

	t2.join();
	printf("t2 join\n");

	t.join();
	printf("t join\n");
}

void ThreadDemo::Mutex()
{
	thread t([&]()
	{
		while (true)
		{
			Sleep(1000);

			m.lock();
			printf("Progress : %f\n", progress);
			m.unlock();
		}
	});
	t.detach();
}

void ThreadDemo::ExecTimer()
{
	timer.Start([]()
	{
		printf("Timer\n");
	}, 2000, 2);

	timer2.Start([]()
	{
		printf("Timer2\n");
	}, 3000);
}

void ThreadDemo::ExecPerfomence()
{
	int arr[10000];
	for (int i = 0; i < 10000; i++)
		arr[i] = Math::Random(0, 100000);

	Performance p;
	p.Start();

	sort(arr, arr + 10000);

	float last = p.End();
	printf("총 수행시간 : %f\n", last);
}
