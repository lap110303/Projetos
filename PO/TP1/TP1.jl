
using JuMP, GLPK

# ===================== CONFIGURAÇÕES INICIAIS =====================

# Tempo total de simulação (em segundos) e passo de tempo
const total_time = 4320 * 60  # 3 dias
const dt = 5                  # intervalo de tempo em segundos
const TEMPOS = 0:dt:(total_time - dt)  # todos os instantes considerados
const CHECKPOINTS = 0:3600:total_time  # pontos a cada hora (usados para controle de produção)

# Recursos e construções possíveis
const RECURSOS = [:wood, :clay, :iron, :crop]
const CAMPOS_RECURSO = [:woodcutter, :claypit, :ironmine, :cropland]
const EDIFICIOS_VALIDOS = [:warehouse, :granary, :barracks, :academy, :smithy, :marketplace,
                           :rally_point, :wall, :stable, :workshop, :townhall, :residence,
                           :palace, :trade_office, :great_barracks]

# Custos e capacidades
const mercenary_cost = Dict(:wood => 95, :clay => 75, :iron => 40, :crop => 40)
const total_cost = Dict(k => 100 * v for (k, v) in mercenary_cost)
const initial_storage = Dict(:wood => 800, :clay => 800, :iron => 800, :crop => 800)
const extra_capacity_per_level = Dict(:warehouse => 1200, :granary => 1200)
const producao_por_nivel = Dict(:woodcutter => 30, :claypit => 30, :ironmine => 20, :cropland => 15)

# ===================== DEFINIÇÃO DO MODELO =====================

model = Model(GLPK.Optimizer)

# Variáveis de decisão
@variable(model, campo[c in CAMPOS_RECURSO, l=1:10], Bin)          # campos de recurso
@variable(model, edificio[b in EDIFICIOS_VALIDOS, l=1:20], Bin)   # edifícios
@variable(model, predios_usados[b in EDIFICIOS_VALIDOS], Bin)     # se o prédio foi utilizado
@variable(model, res[r in RECURSOS, t in CHECKPOINTS] >= 0)       # recurso acumulado
@variable(model, cap[r in RECURSOS, t in CHECKPOINTS] >= 0)       # capacidade de armazenamento
@variable(model, z[t in TEMPOS], Bin)                             # tempo de treinamento escolhido

# ===================== RESTRIÇÕES =====================

# Relação entre uso de edifícios e seus níveis
@constraint(model, [b in EDIFICIOS_VALIDOS], sum(edificio[b, l] for l in 1:20) >= predios_usados[b])
@constraint(model, sum(predios_usados[b] for b in EDIFICIOS_VALIDOS) <= 15)

# Capacidade de armazenamento por tipo de recurso ao longo do tempo
for t in CHECKPOINTS, r in RECURSOS
    base = initial_storage[r]
    extra = r == :crop ? extra_capacity_per_level[:granary] * sum(edificio[:granary, l] for l in 1:20)
                       : extra_capacity_per_level[:warehouse] * sum(edificio[:warehouse, l] for l in 1:20)
    @constraint(model, cap[r, t] == base + extra)
    @constraint(model, res[r, t] <= cap[r, t])
end

# Produção acumulada entre checkpoints
for (i, t) in enumerate(CHECKPOINTS)
    if i == 1 continue end  # pula o tempo 0
    t_prev = CHECKPOINTS[i-1]
    for r in RECURSOS
        prod = sum((producao_por_nivel[c] / 3600) * sum(campo[c, l] for l in 1:10) * (t - t_prev)
                   for c in CAMPOS_RECURSO if (c == :woodcutter && r == :wood) ||
                                               (c == :claypit && r == :clay) ||
                                               (c == :ironmine && r == :iron) ||
                                               (c == :cropland && r == :crop))
        @constraint(model, res[r, t] == res[r, t_prev] + prod)
    end
end

# Apenas um instante de tempo pode ser escolhido para o treinamento
@constraint(model, sum(z[t] for t in TEMPOS) == 1)

# Garante que, no tempo escolhido, os recursos acumulados são suficientes para o custo total
M = 1e5
function closest_checkpoint(t::Int, checkpoints)
    return maximum(filter(c -> c <= t, checkpoints))
end
for r in RECURSOS, t in TEMPOS
    t_c = closest_checkpoint(t, collect(CHECKPOINTS))
    @constraint(model, res[r, t_c] + M * (1 - z[t]) >= total_cost[r])
end

# Barracks deve estar presente
@constraint(model, sum(edificio[:barracks, l] for l in 1:20) >= 1)

# ===================== OBJETIVO =====================

# Maximizar a soma dos níveis construídos (pesos arbitrários para importância relativa)
@objective(model, Max,
    sum(campo[c, l] * 2 for c in CAMPOS_RECURSO, l in 1:10) +
    sum(edificio[b, l] * 3 for b in EDIFICIOS_VALIDOS, l in 1:20))

# ===================== OTIMIZAÇÃO E RESULTADO =====================

optimize!(model)

println("\nConstruções recomendadas:\n-----------------------------")
# Exibe níveis máximos dos campos de recurso
for c in CAMPOS_RECURSO
    niveis = [l for l in 1:10 if value(campo[c, l]) > 0.5]
    if !isempty(niveis)
        println("$(c): até o nível $(maximum(niveis))")
    end
end

# Exibe níveis máximos dos edifícios
for b in EDIFICIOS_VALIDOS
    niveis = [l for l in 1:20 if value(edificio[b, l]) > 0.5]
    if !isempty(niveis)
        println("$(b): até o nível $(maximum(niveis))")
    end
end

# Tempo ótimo para treinamento
chosen_train_time = sum(t * value(z[t]) for t in TEMPOS)
println("\nTempo recomendado para treinar tropas: ", chosen_train_time, " segundos")

# Recursos disponíveis no tempo de treinamento
for r in RECURSOS
    println("  $(r): ", sum(value(res[r, t]) * value(z[t]) for t in TEMPOS))
end

