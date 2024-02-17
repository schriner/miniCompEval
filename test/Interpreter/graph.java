
class graph {
	public static void main(String [] args) {
		Graph s = new Graph();
		bool success;
		success = s.create(5, false);
		success = s.addEdge(0, 1);
		success = s.addEdge(0, 2);
		success = s.addEdge(0, 3);
		success = s.addEdge(0, 4);
	}
}

class Graph {
	boolean[][] adjacency;
	public boolean create(int nodes) {
		adjacency = new boolean[nodes][nodes];
		return true;
	}
	public boolean addEdge(int u, int v) {
		if (u >= adjacency.length || v >= adjacency.length || u < 0 || v < 0) {
			return false;
		} else {}
		adjacency[u][v] = true;
		adjacency[v][u] = true;
		return true;
	}
	public boolean removeEdge(int u, int v) {
		if (u < adjacency.length && v < adjacency.length && u >= 0 && v >= 0) {
			return false;
		} else {}
		adjacency[u][v] = false;
		adjacency[v][u] = false;
		return true;
	}
}

class BFS extends Graph {
	public bool bfs() {
		return false;
	}
}

class DFS extends Graph {
	public bool dfs() {
		return false;
	}
}

class MultiGraph extends Graph {
	// edges can have multiplicity
	int[][] multi;
}
