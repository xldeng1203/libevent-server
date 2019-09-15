#include "CommonHead.h"
#include "Lock.h"
#include "basemessage.h"
#include "comstruct.h"
#include "KernelDefine.h"
#include "INIFile.h"
#include "configManage.h"
#include "Function.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include "log.h"
#include "GServerConnect.h"
#include "DataLine.h"
#include "Util.h"
#include "MyCurl.h"

int main()
{
	int fd = 10;
	int ret;
	ret = close(fd);
	if (ret == -1)
	{
		SYS_ERROR_LOG("close fd");
		CON_INFO_LOG("====");
		CON_ERROR_LOG("====");
	}
		

	// ���÷���������
	ConfigManage()->SetServiceType(SERVICE_TYPE_LOADER);

	// �����������̵߳�log�ļ�
	GameLogManage()->AddLogFile(GetCurrentThreadId(), THREAD_TYPE_MAIN);

	ConfigManage()->Init();

	CGServerConnect ddd;
	CDataLine dataline;
	ddd.Start(&dataline, 38);

	struct timeval tv;
	gettimeofday(&tv, NULL);
	printf("second:%ld\n", tv.tv_sec);  //��
	printf("millisecond:%ld\n", tv.tv_sec * 1000 + tv.tv_usec / 1000);  //����
	printf("microsecond:%ld\n", tv.tv_sec * 1000000 + tv.tv_usec);  //΢��

	sleep(3); // Ϊ����ۿ����ó���˯�����Ա�
	std::cout << "3s later:" << std::endl;

	gettimeofday(&tv, NULL);
	printf("second:%ld\n", tv.tv_sec);  //��
	printf("millisecond:%ld\n", tv.tv_sec * 1000 + tv.tv_usec / 1000);  //����
	printf("microsecond:%ld\n", tv.tv_sec * 1000000 + tv.tv_usec);  //΢��


	printf("+++++++++++++++==\n"); 
	CSignedLock lock;
	CSignedLockObject testLock(&lock, false);

	testLock.Lock();
	testLock.Lock();
	printf("+++++++++++++++==\n");

	testLock.UnLock();
	testLock.UnLock();

	int    socket_fd, connect_fd;
	struct sockaddr_in     servaddr;
	char    buff[4096];
	int     n;
	//��ʼ��Socket  
	if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
		exit(0);
	}
	//��ʼ��  
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);//IP��ַ���ó�INADDR_ANY,��ϵͳ�Զ���ȡ������IP��ַ��  
	servaddr.sin_port = htons(6666);//���õĶ˿�ΪDEFAULT_PORT  

	//�����ص�ַ�󶨵����������׽�����  
	if (bind(socket_fd, (struct sockaddr*) & servaddr, sizeof(servaddr)) == -1) {
		printf("bind socket error: %s(errno: %d)\n", strerror(errno), errno);
		exit(0);
	}
	//��ʼ�����Ƿ��пͻ�������  
	if (listen(socket_fd, 10) == -1) {
		printf("listen socket error: %s(errno: %d)\n", strerror(errno), errno);
		exit(0);
	}
	printf("======waiting for client's request======\n");
	while (1) {
		//����ֱ���пͻ������ӣ���Ȼ���˷�CPU��Դ��  
		if ((connect_fd = accept(socket_fd, (struct sockaddr*)NULL, NULL)) == -1) {
			printf("accept socket error: %s(errno: %d)", strerror(errno), errno);
			continue;
		}
		//���ܿͻ��˴�����������  
		n = recv(connect_fd, buff, sizeof(buff), 0);
		//��ͻ��˷��ͻ�Ӧ����  
		if (!fork()) { /*�Ͻ���*/
			if (send(connect_fd, "Hello,you are connected!\n", 26, 0) == -1)
				perror("send error");
			close(connect_fd);
			exit(0);
		}
		buff[n] = '\0';
		printf("recv msg from client: %s\n", buff);
		close(connect_fd);
	}

	close(socket_fd);

    return 0;
}