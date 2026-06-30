CC = gcc
CFLAGS = -Wall -Wextra -std=c11

# Varsayılan kural: 'make' yazıldığında tüm programları derler
all: ServerTrd trader analyst ticker

# Sunucu: Ana program ve fonksiyon tanımlarının olduğu defs.c birlikte derlenir
ServerTrd: ServerTrd.c defs.c defs.h
	$(CC) $(CFLAGS) -o ServerTrd ServerTrd.c defs.c

# Trader: Sadece trader.c derlenir
trader: trader.c defs.h
	$(CC) $(CFLAGS) -o trader trader.c

# Analyst: Sadece analyst.c derlenir
analyst: analyst.c defs.h
	$(CC) $(CFLAGS) -o analyst analyst.c

# Ticker: Sadece ticker.c derlenir
ticker: ticker.c defs.h
	$(CC) $(CFLAGS) -o ticker ticker.c

# Temizlik kuralı: 'make clean' yazıldığında derlenmiş dosyaları ve .o dosyalarını siler
clean:
	rm -f ServerTrd trader analyst ticker *.o