# Biblioteca de músicas em C — Estruturas de Dados II

Este projeto faz parte da terceira avaliação de **Estruturas de Dados II (ED2)** do curso de Sistemas de Informação da Universidade Federal do Piauí (UFPI), no Campus Senador Helvídio Nunes de Barros. A lista propõe implementar a mesma biblioteca de músicas com três tipos de árvore de busca e comparar o custo de cadastrar e localizar artistas em cada versão.

## Questões da lista

Cada questão vale **2,5 pontos**, totalizando 10 pontos.

| Questão | O que a lista pede | Situação |
| --- | --- | --- |
| **1. Árvore binária de busca (ABB)** | Implementar a biblioteca em C usando árvores de busca comuns para organizar artistas, álbuns, músicas e playlists. | Implementada e testada |
| **2. Árvore AVL** | Reimplementar as mesmas funcionalidades usando árvores AVL, que controlam o balanceamento pela altura das subárvores. | Planejada |
| **3. Árvore rubro-negra (vermelha-preta)** | Reimplementar a biblioteca usando árvores rubro-negras. A variante prevista no projeto é a **LLRB**, a rubro-negra pendente para a esquerda estudada na disciplina. | Planejada |
| **4. Comparação de desempenho** | Medir e comparar as inserções e buscas na árvore de artistas das três versões. | Planejada |

## Funcionamento da biblioteca

Cada artista tem sua própria árvore de álbuns, e cada álbum tem sua própria árvore de músicas. Há também uma árvore de playlists. Cada playlist mantém uma árvore de itens que identificam músicas do catálogo pelo artista, pelo álbum e pelo título da faixa.

- **Cadastros (itens i a iii).** O sistema registra o nome, o tipo e o estilo musical do artista. Para cadastrar um álbum, o artista precisa existir. Para cadastrar uma música, o álbum precisa existir. Os nomes de artistas são únicos, e a verificação de títulos repetidos respeita o contexto de cada artista ou álbum.
- **Consultas (itens iv a vi).** É possível listar todos os artistas, consultar os álbuns de um artista lançados em determinado ano e buscar uma música pelo título. A busca mostra as ocorrências com o artista, seu tipo e estilo musical, o álbum e o ano de lançamento.
- **Playlists (itens vii e viii).** O usuário pode criar playlists com nomes únicos, adicionar músicas cadastradas e consultar os dados de cada playlist.
- **Remoções (itens ix e x).** Excluir uma faixa da playlist mantém a música no catálogo. A exclusão da música de seu álbum só é permitida quando ela já não pertence a nenhuma playlist.
