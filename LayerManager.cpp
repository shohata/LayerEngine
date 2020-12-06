#include "interface.h"

#define ADDMAX 128

LayerManager::LayerManager()
{
	contents = (Layer**)malloc(sizeof(Layer*) * ADDMAX);
	dwLayerCount = 0;
	dwMax = ADDMAX;
}

LayerManager::~LayerManager()
{
	RemoveAll();
	free(contents);
}

int LayerManager::Add(Layer *layer)
{
	Insert(layer , dwLayerCount);
	return dwLayerCount;
}

bool LayerManager::Insert(Layer *layer, int index)
{
	int i;

	if (dwMax == dwLayerCount) {
		dwMax += ADDMAX;
		contents = (Layer **)realloc(contents , sizeof(Layer*) * dwMax);
	}

	for(i = dwLayerCount ; i > index ; i--) {
		contents[i] = contents[i - 1];
	}

	contents[index] = layer;
	dwLayerCount++;
	return true;
}

bool LayerManager::Remove(Layer* layer) {
	int i;

	for(i = 0 ; i < dwLayerCount ; i++) {
		if (contents[i] == layer) {
			RemoveAt(i);
			return TRUE;
		}
	}

	return false;
}

void LayerManager::RemoveAt(int index) {
	int i;

	if (index >= dwLayerCount) return;

	dwLayerCount--;

	for(i = index ; i < dwLayerCount ; i++) {
		contents[i] = contents[i + 1];
	}
}

void LayerManager::RemoveAll() {
	for(; dwLayerCount ; ) {
		RemoveAt(0);
	}
}

Layer* LayerManager::GetLayer(int index) {
	if (index >= dwLayerCount) return NULL;
	return contents[index];
}

int LayerManager::GetCount() {
	return dwLayerCount;
}