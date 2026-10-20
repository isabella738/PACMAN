
COMPILADOR = g++
CXXFLAGS = -Iinclude -g -O0

# Diretorio dos arquivos objeto (.o)
OBJ_DIR = objects

# Todos os arquivos .cpp do projeto
FONTES = $(shell find source -type f -name "*.cpp")

# Caminhos dos arquivos objeto
OBJ = $(patsubst source/%.cpp,$(OBJ_DIR)/%.o,$(FONTES))

# Todos os cabecalhos
CABECALHOS = $(shell find source include -type f \( -name "*.h" -o -name "*.hpp" \) 2>/dev/null)

# Regra de compilacao dos .cpp
$(OBJ_DIR)/%.o: source/%.cpp $(CABECALHOS)
	@mkdir -p $(dir $@)
	$(COMPILADOR) $(CXXFLAGS) -c $< -o $@

# Linkagem
PACMAN: $(OBJ)
	$(COMPILADOR) -o $@ $^

.PHONY: clean

clean:
	rm -rf $(OBJ_DIR) PACMAN
