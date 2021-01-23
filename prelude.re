(def [nil] [])
(def [true] 1)
(def [false] 0)

(def [fn] (lambda [f b] [
  def (head f) (lambda (tail f) b)
  ]))


(fn [unpack f l] [
     eval (join (list f) l)
     ])

(fn [pack f & xs] [f xs])

(def [curry] unpack)
(def [uncurry] pack)

(fn [do & l] [
    if (== l nil)
    [nil]
    [last l]
    ])

(fn [let b] [
    ((lambda [_] b) ())
    ])

(fn [flip f a b] [f b a])

(fn [ghost & xs] [eval xs])

(fn [comp f g x] [f (g x)])

(fn [fst l] [eval (head l)])
(fn [snd l] [eval (head (tail l))])
(fn [trd l] [eval (head (tail (tail l)))])
(fn [len l] [
    if (== l nil)
    [0]
    [+ 1 (len (tail l))]
    ])
(fn [nth n l] [
    if (== n 0)
    [fst l]
    [nth (- n 1) (tail l)]
    ])

(fn [last l] [nth (- (len l) 1) l])

(fn [take n l] [
    if (== n 0)
    [nil]
    [join (head l) (take (- n 1) (tail l))]
    ])

(fn [drop n l] [
    if (== n 0)
    [l]
    [drop (- n 1) (tail l)]
    ])

(fn [map f l] [
    if (== l nil)
    [nil]
    [join (list (f (fst l))) (map f (tail l))]
    ])

(fn [filter f l] [
    if (== l nil)
    [nil]
    [join (if (f (fst l)) [head l] [nil]) (filter f (tail l))]
    ])

(fn [select & cs] [
    if (== cs nil)
        [error "No cases"]
      [if (fst (fst cs))
          [snd (fst cs)]
        [unpack select (tail cs)]]
    ])

(fn [case x default & cs] [
    if (== cs nil)
    [default]
    [if (== x (fst (fst cs)))
        [snd (fst cs)]
      [unpack case (join (list x default) (tail cs))]]
    ])

(fn [hi] [print "Hi"])

(fn [const2] [2])
