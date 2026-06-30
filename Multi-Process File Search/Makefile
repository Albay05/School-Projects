# Derleyici ve Bayraklar
CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99 -g
LDFLAGS = 

# Hedef dosya adı
TARGET = procSearch

# Kaynak ve Obje dosyaları
SRCS = main.c util.c
OBJS = $(SRCS:.c=.o)
HEADERS = util.h

# Varsayılan hedef
all: $(TARGET)

# Bağlama (Linking) aşaması
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Derleme (Compiling) aşaması
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Temizlik (Cleanup)
clean:
	rm -f $(OBJS) $(TARGET) worker_*.txt

# Projeyi yeniden derle
re: clean all

.PHONY: all clean re