#include "stats.h"

void* RAI_AddStatsEntry(RedisModuleCtx* ctx, RedisModuleString* key, RAI_RunType runtype,
                        RAI_Backend backend, const char* devicestr, const char* tag) {
  const char* infokey = RedisModule_StringPtrLen(key, NULL);

  struct RedisAI_RunStats *rstats = NULL;
  rstats = RedisModule_Calloc(1, sizeof(struct RedisAI_RunStats));
  RedisModule_RetainString(ctx, key);
  rstats->key = key;
  rstats->type = runtype;
  rstats->backend = backend;
  rstats->devicestr = RedisModule_Strdup(devicestr);
  rstats->tag = RedisModule_Strdup(tag);

  AI_dictAdd(run_stats, (void*)infokey, (void*)rstats);

  return (void*)infokey;
}

void RAI_ListStatsEntries(RAI_RunType type, long long* nkeys, RedisModuleString*** keys,
                          const char*** tags) {
  AI_dictIterator *stats_iter = AI_dictGetSafeIterator(run_stats);

  long long stats_size = AI_dictSize(run_stats);

  *keys = RedisModule_Calloc(stats_size, sizeof(RedisModuleString*));
  *tags = RedisModule_Calloc(stats_size, sizeof(const char*));

  *nkeys = 0;

  AI_dictEntry *stats_entry = AI_dictNext(stats_iter);
  struct RedisAI_RunStats *rstats = NULL;

  while (stats_entry) {
    rstats = AI_dictGetVal(stats_entry);

    if (rstats->type == type) {
      (*keys)[*nkeys] = rstats->key;
      (*tags)[*nkeys] = rstats->tag;
      *nkeys += 1;
    }

    stats_entry = AI_dictNext(stats_iter);
  }

  AI_dictReleaseIterator(stats_iter);
}

void RAI_RemoveStatsEntry(void* infokey) {
  AI_dictEntry *stats_entry = AI_dictFind(run_stats, infokey);

  if (stats_entry) {
    struct RedisAI_RunStats *rstats = AI_dictGetVal(stats_entry);
    AI_dictDelete(run_stats, infokey);
    RAI_FreeRunStats(rstats);
    RedisModule_Free(rstats);
  }
}

void RAI_FreeRunStats(struct RedisAI_RunStats *rstats) {
  RedisModule_Free(rstats->devicestr);
  RedisModule_Free(rstats->tag);
}

