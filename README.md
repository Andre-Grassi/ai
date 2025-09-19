# AI Search Algorithms Documentation

This repository contains implementations of various AI search algorithms with comprehensive documentation hosted on GitHub Pages.

## 📖 Documentation

The complete API documentation is available at:
**https://andre-grassi.github.io/ai/**

The documentation includes:
- Class hierarchies and inheritance diagrams
- Detailed API reference for all classes and methods
- Code examples and usage patterns
- Search algorithm implementations (BFS, UCS, A*, etc.)
- Problem formulations (Sliding Tile, etc.)

## 🏗️ Building Documentation Locally

If you want to build the documentation locally:

```bash
cd search_algorithm/c++
doxygen Doxyfile
```

The generated documentation will be in `search_algorithm/c++/docs/html/index.html`.

### Prerequisites

- Doxygen (`sudo apt install doxygen` on Ubuntu)
- Graphviz for diagrams (`sudo apt install graphviz`)

## 🚀 Automatic Deployment

Documentation is automatically built and deployed to GitHub Pages when:
- Changes are pushed to the `main` branch
- Changes are pushed to the `docs` branch
- Pull requests are made to `main`

The deployment uses GitHub Actions and the workflow is defined in `.github/workflows/deploy-docs.yml`.

## 📁 Repository Structure

```
search_algorithm/
├── c++/
│   ├── algorithms/           # Search algorithm implementations
│   ├── data_structure/      # Node, Problem, and comparator classes
│   │   ├── node.h          # Search tree node
│   │   ├── problem.h       # Abstract problem interface
│   │   ├── node_comparators.h  # Priority queue comparators
│   │   └── problems/       # Specific problem implementations
│   ├── docs/               # Generated documentation (local build)
│   ├── Doxyfile           # Doxygen configuration
│   └── Makefile           # Build system
└── python/                # Python implementations (if any)
```

## 🔍 Key Classes

- **Problem**: Abstract base class for search problems
- **Node**: Represents states in the search tree
- **SlidingTileProblem**: 8-puzzle implementation
- **Comparators**: Priority queue ordering for different algorithms

## 🤝 Contributing

When adding new code:

1. Add comprehensive Doxygen comments
2. Include `@brief`, `@param`, `@return` documentation
3. Document template parameters with `@tparam`
4. Push to trigger automatic documentation updates