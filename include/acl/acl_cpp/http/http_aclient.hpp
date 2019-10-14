#pragma once
#include "../acl_cpp_define.hpp"
#include "../stream/aio_socket_stream.hpp"

struct HTTP_HDR;
struct HTTP_HDR_RES;
struct HTTP_RES;
struct HTTP_HDR_REQ;
struct HTTP_REQ;

struct ACL_ASTREAM;

namespace acl {

class aio_handle;
class aio_socket_stream;
class socket_stream;
class zlib_stream;
class websocket;
class polarssl_conf;
class polarssl_io;
class http_header;

/**
 * HTTP �ͻ����첽ͨ���࣬����֧�ֱ�׼ HTTP ͨ��Э�飬����֧�� Websocket ͨ�ţ�
 * ���� HTTP Э�鼰 Websocket ͨ�ž�֧�� SSL ���ܴ��䣻
 * ���⣬���� HTTP Э�飬�����û����ã������Զ���ѹ GZIP ��Ӧ���ݣ������ڻص�
 * ���� on_http_res_body() ���յ��ı��ǽ�ѹ����������ݡ�
 */
class ACL_CPP_API http_aclient : public aio_open_callback
{
public:
	/**
	 * ���캯��
	 * @param handle {aio_handle&} �첽ͨ���¼�������
	 * @param ssl_conf {polarssl_conf*} �� NULL ʱ�Զ����� SSL ͨ�ŷ�ʽ
	 */
	http_aclient(aio_handle& handle, polarssl_conf* ssl_conf = NULL);
	virtual ~http_aclient(void);

	/**
	 * ����������ʱ�Ļص��������������ʵ��
	 */
	virtual void destroy(void) = 0;

	/**
	 * ��� HTTP ����ͷ���Ա���Ӧ����� HTTP ����ͷ�е��ֶ�����
	 * @return {http_header&}
	 */
	http_header& request_header(void);

	/**
	 * ��� HTTP Э�����Ӧ�����Ƿ��Զ����н�ѹ
	 * @param on {bool}
	 * @return {http_aclient&}
	 */
	http_aclient& unzip_body(bool on);

	/**
	 * �Ƿ���� GZIP ѹ�������Զ����н�ѹ
	 * @return {bool}
	 */
	bool is_unzip_body(void) const
	{
		return unzip_;
	}

	/**
	 * ��ʼ�첽����Զ�� WEB ������
	 * @param addr {const char*} Զ�� WEB ��������ַ����ʽΪ��
	 *  domain:port �� ip:port, ����ַΪ����ʱ���ڲ��Զ������첽��������
	 *  ���̣���Ҫ���ڳ���ʼʱ����ͨ�� aio_handle::set_dns() ���ù�����
	 *  ��������ַ�������ַΪ IP ����Ҫ������������������ַ
	 * @param conn_timeout {int} ���ӳ�ʱʱ�䣨�룩
	 * @param rw_timeout {int} ���� IO ��д��ʱʱ�䣨�룩
	 * @return {bool} ���� false ��ʾ����ʧ�ܣ����� true ��ʾ�����첽������
	 */
	bool open(const char* addr, int conn_timeout, int rw_timeout);

protected:
	/**
	 * �����ӳɹ���Ļص��������������ʵ�֣�����Ӧ�ڸ÷����ﹹ�� HTTP ����
	 * ������ send_request ������ WEB ���������� HTTP ����
	 * @return {bool} �÷���������� false ���ڲ����Զ��ر�����
	 */
	virtual bool on_connect(void) = 0;

	/**
	 * �����ӳ�ʱ��Ļص�����
	 */
	virtual void on_connect_timeout(void) {}

	/**
	 * ������ʧ�ܺ�Ļص�����
	 */
	virtual void on_connect_failed(void) {}

	/**
	 * ���������ʱʱ�Ļص�����
	 */
	virtual void on_read_timeout(void) {}

	/**
	 * �������ӳɹ������ӹرպ�Ļص�����
	 */
	virtual void on_disconnect(void) {};

	/**
	 * �����յ� WEB ����˵���Ӧͷʱ�Ļص�����
	 * @param header {const http_header&}
	 * @return {bool} ���� false �򽫻�ر����ӣ����������
	 */
	virtual bool on_http_res_hdr(const http_header& header)
	{
		(void) header;
		return true;
	}

	/**
	 * �����յ� WEB ����˵���Ӧ��ʱ�Ļص��������÷������ܻᱻ��λص�
	 * ֱ����Ӧ���ݶ�������
	 * @param data {char*} �����Ĳ�������������
	 * @param dlen {size_t} ���ζ����� data ���ݵĳ���
	 * @return {bool} ���� false �򽫻�ر����ӣ����������
	 */
	virtual bool on_http_res_body(char* data, size_t dlen)
	{
		(void) data;
		(void) dlen;
		return true;
	}

	/**
	 * ������ HTTP ��Ӧ�������Ļص�����
	 * @param success {bool} �Ƿ�ɹ����� HTTP ��Ӧ������
	 * @return {bool} ����ɹ�����������󷵻� false ���ر�����
	 */
	virtual bool on_http_res_finish(bool success)
	{
		(void) success;
		return true;
	}

	/**
	 * �� websocket ���ֳɹ���Ļص�����
	 * @return {bool} ���� false ��ʾ��Ҫ�ر����ӣ��������
	 */
	virtual bool on_ws_handshake(void)
	{
		// ��ʼ�첽�� websocket ����
		this->ws_read_wait(0);
		return true;
	}

	/**
	 * �� websocket ����ʧ�ܺ�Ļص�����
	 * @param status {int} ���������ص� HTTP ��Ӧ״̬��
	 */
	virtual void on_ws_handshake_failed(int status) { (void) status; }

	/**
	 * ������һ�� text ���͵�֡ʱ�Ļص�����
	 * @return {bool} ���� true ��ʾ��������������Ҫ��ر�����
	 */
	virtual bool on_ws_frame_text(void) { return true; }

	/**
	 * ������һ�� binary ���͵�֡ʱ�Ļص�����
	 * @return {bool} ���� true ��ʾ��������������Ҫ��ر�����
	 */
	virtual bool on_ws_frame_binary(void) { return true; }

	/**
	 * ������һ���ر�֡����ʱ�Ļص�����
	 */
	virtual void on_ws_frame_closed(void) {}

	/**
	 * �� websocket ͨ�ŷ�ʽ��������������ʱ�Ļص�����
	 * @param data {char*} ���������ݵ�ַ
	 * @param dlen {size_t} ���������ݳ���
	 * @return {bool} ���� true ��ʾ��������������Ҫ��ر�����
	 */
	virtual bool on_ws_frame_data(char* data, size_t dlen)
	{
		(void) data;
		(void) dlen;
		return true;
	}

	/**
	 * ������һ֡����ʱ�Ļص�����
	 * @return {bool} ���� true ��ʾ��������������Ҫ��ر�����
	 */
	virtual bool on_ws_frame_finish(void) { return true; }

public:
	/**
	 * �� WEB ���������� HTTP �����ڲ��ڷ��ͺ���Զ���ʼ�� HTTP ��Ӧ����
	 * @param body {const void*} HTTP ����������壬��Ϊ NULL ʱ���ڲ�����
	 *  ������ HTTP GET ����
	 * @param len {size_t} body �� NULL ʱ��ʾ������ĳ���
	 */
	void send_request(const void* body, size_t len);

	/**
	 * ����������� WEBSOCKET ����
	 */
	void ws_handshake(void);

	/**
	 * ��ʼ�첽�� websocket ����
	 * @param timeout {int} ����ʱʱ��
	 */
	void ws_read_wait(int timeout = 0);

	/**
	 * �첽����һ�� FRAME_TEXT ���͵�����֡
	 * @param data {char*} �ڲ��������������ԭ�򱻸ı�����
	 * @param len {size_t} data ���ݳ���
	 * @return {bool}
	 */
	bool ws_send_text(char* data, size_t len);

	/**
	 * �첽����һ�� FRAME_BINARY ���͵�����֡
	 * @param data {void*} �ڲ��������������ԭ�򱻸ı�����
	 * @param len {size_t} data ���ݳ���
	 * @return {bool}
	 */
	bool ws_send_binary(void* data, size_t len);

	/**
	 * �첽����һ�� FRAME_PING ���͵�����֡
	 * @param data {void*} �ڲ��������������ԭ�򱻸ı�����
	 * @param len {size_t} data ���ݳ���
	 * @return {bool}
	 */
	bool ws_send_ping(void* data, size_t len);

	/**
	 * �첽����һ�� FRAME_PONG ���͵�����֡
	 * @param data {void*} �ڲ��������������ԭ�򱻸ı�����
	 * @param len {size_t} data ���ݳ���
	 * @return {bool}
	 */
	bool ws_send_pong(void* data, size_t len);

protected:
	// @override dummy
	bool open_callback(void) { return true; }

	// @override
	bool timeout_callback(void);

	// @override
	void close_callback(void);

	// @override
	bool read_wakeup(void);

	// @override
	bool read_callback(char* data, int len);

protected:
	unsigned           status_;
	aio_handle&        handle_;
	polarssl_conf*     ssl_conf_;
	int                rw_timeout_;
	aio_socket_stream* conn_;
	socket_stream*     stream_;
	http_header*       header_;
	HTTP_HDR_RES*      hdr_res_;
	HTTP_RES*          http_res_;
	bool               keep_alive_;
	websocket*         ws_in_;
	websocket*         ws_out_;
	string*            buff_;
	bool               unzip_;		// �Ƿ��Զ���ѹ��Ӧ������
	zlib_stream*       zstream_;		// ��ѹ����
	int                gzip_header_left_;	// gzip ����ʱѹ��ͷ������

	bool handle_connect(ACL_ASTREAM* stream);
	bool handle_ssl_handshake(void);

	bool handle_res_hdr(int status);

	bool handle_res_body(char* data, int dlen);
	bool res_plain(char* data, int dlen);
	bool res_unzip(zlib_stream& zstream, char* data, int dlen);

	bool handle_res_body_finish(char* data, int dlen);
	bool res_plain_finish(char* data, int dlen);
	bool res_unzip_finish(zlib_stream& zstream, char* data, int dlen);

	bool handle_websocket(void);
	bool handle_ws_data(void);
	bool handle_ws_ping(void);
	bool handle_ws_pong(void);
	bool handle_ws_other(void);

private:
	static int connect_callback(ACL_ASTREAM* stream, void* ctx);
	static int http_res_hdr_cllback(int status, void* ctx);
	static int http_res_callback(int status, char* data, int dlen, void* ctx);
};

} // namespace acl
