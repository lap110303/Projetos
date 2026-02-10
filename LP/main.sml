structure Main =
struct
  structure II = IntInf

  fun solve () =
    let
      val NUM_DIGITOS = 20
      val MAX_SOMA_SQ = 1620
      val MODULO = II.fromInt(1000000000)

      fun power (base, exp) =
        if exp = 0 then II.fromInt(1)
        else let
          val p = power(base, exp div 2)
          val p_squared = II.*(p, p)
        in
          if exp mod 2 = 0 then p_squared
          else II.*(base, p_squared)
        end

      val potenciasDe10 = Array.tabulate(NUM_DIGITOS, fn i => power(II.fromInt(10), i))

      fun gerarQuadrados (i, acc) =
        let val q = i * i in
          if q > MAX_SOMA_SQ then acc else gerarQuadrados(i + 1, q :: acc)
        end
      val quadradosPerfeitos = gerarQuadrados(1, [])

      fun loop_digitos (d, contagens_ant, somas_ant) =
        if d > NUM_DIGITOS then
          (contagens_ant, somas_ant)
        else
          let
            val contagens_atuais = Array.array(MAX_SOMA_SQ + 1, II.fromInt(0))
            val somas_atuais = Array.array(MAX_SOMA_SQ + 1, II.fromInt(0))
            val pot10 = Array.sub(potenciasDe10, d - 1)

            fun loop_somas (s_ant) =
              if s_ant > (d-1) * 81 then ()
              else
                let
                  val cont_ant = Array.sub(contagens_ant, s_ant)
                in
                  if cont_ant = II.fromInt(0) then
                    loop_somas(s_ant + 1)
                  else
                    let
                      val soma_ant = Array.sub(somas_ant, s_ant)
                      fun loop_digito (k) =
                        if k > 9 then ()
                        else
                          let
                            val s_nova = s_ant + k * k
                            val cont_atual = Array.sub(contagens_atuais, s_nova)
                            val soma_atual = Array.sub(somas_atuais, s_nova)
                            val nova_cont = II.mod(II.+(cont_atual, cont_ant), MODULO)
                            val () = Array.update(contagens_atuais, s_nova, nova_cont)

                            val k_ii = II.fromInt(k)
                            val contrib_soma = II.mod(II.*(cont_ant, II.*(k_ii, pot10)), MODULO)
                            val nova_soma = II.mod(II.+(soma_atual, II.+(soma_ant, contrib_soma)), MODULO)
                            val () = Array.update(somas_atuais, s_nova, nova_soma)
                          in
                            loop_digito(k + 1)
                          end
                    in
                      loop_digito(0);
                      loop_somas(s_ant + 1)
                    end
                end
            val () = loop_somas(0)
          in
            loop_digitos(d + 1, contagens_atuais, somas_atuais)
          end

      val contagens_base = Array.array(MAX_SOMA_SQ + 1, II.fromInt(0))
      val somas_base = Array.array(MAX_SOMA_SQ + 1, II.fromInt(0))
      val () = Array.update(contagens_base, 0, II.fromInt(1))

      val (_, somas_finais) = loop_digitos(1, contagens_base, somas_base)

      fun somarResultados (quadrados, acc) =
        case quadrados of
          [] => acc
        | q::qs =>
            let
              val soma_para_q = Array.sub(somas_finais, q)
              val novo_acc = II.mod(II.+(acc, soma_para_q), MODULO)
            in
              somarResultados(qs, novo_acc)
            end
    in
      somarResultados(quadradosPerfeitos, II.fromInt(0))
    end
end