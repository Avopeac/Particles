AppendStructuredBuffer<uint> particleDeadList;

[numthreads(1024, 1, 1)]
void CSEntry(uint3 id : SV_DispatchThreadID) {
	// NOTE: Add all available particles to the dead list, since they have yet to spawn
	particleDeadList.Append(id.x);
}