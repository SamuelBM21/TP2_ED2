#!/usr/bin/env python3
import subprocess
import re
import os
import sys
import csv # Usaremos o módulo padrão de CSV
import datetime # Adicione esta importação
import locale   # Adicione esta importação

# --- Configuração dos Testes ---

# O nome do seu programa executável
EXECUTAVEL = './ordena'

# Mapeamento dos códigos para nomes legíveis
METODOS = {
    '1': 'Intercalacao (Quicksort)',
    '2': 'Intercalacao (Selecao por Substituicao)',
    '3': 'Quicksort Externo'
}
SITUACOES = {
    '1': 'Ascendente',
    '2': 'Descendente',
    '3': 'Aleatorio'
}

# Variações de quantidade de registros
QUANTIDADES = [100, 1000, 10000, 100000, 471705]

# Nome do arquivo de saída
ARQUIVO_SAIDA_CSV = 'resultados_testes.csv'

# --- Funções Auxiliares ---

def extrair_valor(padrao_regex, texto_saida):
    """
    Usa uma expressão regular para encontrar um padrão na saída de texto
    e extrair o valor numérico (inteiro ou float).
    """
    match = re.search(padrao_regex, texto_saida)
    if not match:
        return "N/A" # Retorna 'Não aplicável' se não encontrar
    
    valor_str = match.group(1)
    try:
        # Tenta converter para inteiro, se falhar, tenta para float
        return int(valor_str)
    except ValueError:
        return float(valor_str)

def imprimir_tabela(lista_de_dicionarios, headers):
    """
    Imprime uma tabela formatada no console.
    """
    # Calcula a largura máxima de cada coluna
    col_widths = {h: len(h) for h in headers}
    for row in lista_de_dicionarios:
        for h in headers:
            col_widths[h] = max(col_widths[h], len(str(row[h])))

    # Cria e imprime a linha de cabeçalho
    header_line = " | ".join(f"{h:<{col_widths[h]}}" for h in headers)
    print(header_line)
    print("-" * len(header_line)) # Linha separadora

    # Imprime cada linha de dados
    for row in lista_de_dicionarios:
        row_line = " | ".join(f"{str(row[h]):<{col_widths[h]}}" for h in headers)
        print(row_line)

# --- Script Principal ---

def main():
    """
    Função principal que executa a bateria de testes.
    """
    # Tenta configurar o idioma para Português do Brasil para exibir o mês corretamente
    try:
        locale.setlocale(locale.LC_TIME, 'pt_BR.UTF-8')
    except locale.Error:
        print("Aviso: Locale 'pt_BR.UTF-8' não encontrado. O mês pode ser exibido em inglês.")

    if not os.path.exists(EXECUTAVEL):
        print(f"ERRO: O arquivo executável '{EXECUTAVEL}' não foi encontrado.")
        print("Certifique-se de que o programa C foi compilado e está na mesma pasta.")
        sys.exit(1)

    todos_os_resultados = []
    
    # --- Linha Corrigida ---
    data_hoje = datetime.datetime.now().strftime('%d de %B de %Y')

    print("="*50)
    print(f"Iniciando bateria de testes... (Hoje é {data_hoje})")
    print("="*50)

    # O resto do loop de testes continua exatamente o mesmo...
    for cod_metodo, nome_metodo in METODOS.items():
        for quantidade in QUANTIDADES:
            for cod_situacao, nome_situacao in SITUACOES.items():
                
                print(f"Executando: {nome_metodo} | {quantidade} registros | Ordem: {nome_situacao}")

                comando = [EXECUTAVEL, cod_metodo, str(quantidade), cod_situacao]

                try:
                    resultado_processo = subprocess.run(
                        comando, capture_output=True, text=True, check=True, encoding='utf-8'
                    )

                    output_texto = resultado_processo.stdout

                    resultado_atual = {
                        'Metodo': nome_metodo,
                        'Quantidade': quantidade,
                        'Situacao Arquivo': nome_situacao,
                        'Comparacoes': extrair_valor(r"N[úu]mero de Comparaç[õo]es do M[ée]todo: ([\d\.]+)", output_texto),
                        'Leituras (Chamadas)': extrair_valor(r"N[úu]mero de Leituras do M[ée]todo: ([\d\.]+)", output_texto),
                        'Escritas (Chamadas)': extrair_valor(r"N[úu]mero de Escritas do M[ée]todo: ([\d\.]+)", output_texto),
                        'Tempo Metodo (s)': extrair_valor(r"Tempo de Execuç[ãa]o do M[ée]todo: ([\d\.]+)", output_texto),
                        'Tempo Total (s)': extrair_valor(r"Tempo de Execuç[ãa]o Total: ([\d\.]+)", output_texto)
                    }
                    todos_os_resultados.append(resultado_atual)

                except subprocess.CalledProcessError as e:
                    print(f"  ERRO ao executar o comando: {' '.join(comando)}")
                    print(f"  Saída do erro:\n{e.stderr}")
                except Exception as e:
                    print(f"  Um erro inesperado ocorreu: {e}")

    # O resto da função para salvar em CSV e imprimir a tabela continua o mesmo...
    if not todos_os_resultados:
        print("\nNenhum resultado foi coletado. Verifique os erros acima.")
        return

    try:
        headers = list(todos_os_resultados[0].keys())
        
        with open(ARQUIVO_SAIDA_CSV, 'w', newline='', encoding='utf-8') as f_output:
            writer = csv.DictWriter(f_output, fieldnames=headers)
            writer.writeheader()
            writer.writerows(todos_os_resultados)
            
        print("\n" + "="*50)
        print("Testes concluídos com sucesso!")
        print(f"Resultados salvos no arquivo: '{ARQUIVO_SAIDA_CSV}'")
        print("="*50)
        
        imprimir_tabela(todos_os_resultados, headers)

    except Exception as e:
        print(f"\nOcorreu um erro ao salvar o arquivo CSV ou imprimir a tabela: {e}")

if __name__ == '__main__':
    main()