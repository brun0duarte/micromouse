"""Le o serial da ESP32 e entrega as linhas para a pagina do visualizador.

Rodar:  ~/.platformio/penv/bin/python visualizador/servidor.py [porta serial]
Abrir:  http://localhost:8000
Pare o servidor (Ctrl+C) antes de gravar firmware ou abrir o Serial Monitor.
"""
import http.server
import pathlib
import queue
import sys
import threading
import time

import serial

PORTA = sys.argv[1] if len(sys.argv) > 1 else "/dev/ttyACM0"
PAGINA = pathlib.Path(__file__).with_name("index.html")
clientes = []  # uma fila por aba aberta


def ler_serial():
    while True:
        try:
            with serial.Serial(PORTA, 115200, timeout=1) as s:
                print("serial conectada:", PORTA)
                while True:
                    linha = s.readline().decode(errors="replace").strip()
                    if linha:
                        for fila in list(clientes):
                            try:
                                fila.put_nowait(linha)
                            except queue.Full:  # aba travada: descarta
                                pass
        except serial.SerialException as e:
            print("serial:", e, "- tentando de novo em 1 s")
            time.sleep(1)


class Servidor(http.server.BaseHTTPRequestHandler):
    def do_GET(self):
        if self.path == "/dados":
            self.send_response(200)
            self.send_header("Content-Type", "text/event-stream")
            self.send_header("Cache-Control", "no-cache")
            self.end_headers()
            fila = queue.Queue(maxsize=1000)
            clientes.append(fila)
            try:
                while True:
                    self.wfile.write(f"data: {fila.get()}\n\n".encode())
                    self.wfile.flush()
            except (BrokenPipeError, ConnectionResetError):
                pass
            finally:
                clientes.remove(fila)
        else:
            self.send_response(200)
            self.send_header("Content-Type", "text/html; charset=utf-8")
            self.end_headers()
            self.wfile.write(PAGINA.read_bytes())

    def log_message(self, *args):
        pass


threading.Thread(target=ler_serial, daemon=True).start()
print("abra http://localhost:8000")
http.server.ThreadingHTTPServer(("127.0.0.1", 8000), Servidor).serve_forever()
