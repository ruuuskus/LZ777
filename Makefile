CC = gcc
CFLAGS = -std=c90 -pedantic -Wall -O2
TARGET = lz77
OBJS = lz77.o make_lz77.o



all: $(TARGET)

# получение цели     $@ (вставляет то что в TARGET (имя цели))    $^ (все зависимости (вставляет то что в OBJS) )
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# -c (только компиляция, без линковка)    $< (первая зависимость (в данном случаи это lz77.c ))
lz77.o: lz77.c lz77os.h
	$(CC) $(CFLAGS) -c $<

make_lz77.o: make_lz77.c lz77os.h
	$(CC) $(CFLAGS) -c $<


# очистка (выполняеться всегда, тк нет зависимости)   rm -f (команда принудительного удаления)
clean:
	rm -f $(OBJS) $(TARGET)

# all and clean это не имена файлов, а команды
.PHONY: all clean