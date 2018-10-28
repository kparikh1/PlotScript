(begin
(define make-point (lambda (x y) (set-property "object-name" "point" (set-property "size" 0 (list x y)))))
(define make-line (lambda (x y) (set-property "object-name" "line" (set-property "thickness" 1 (list x y)))))
(define make-text (lambda (x) (set-property "object-name" "text" (set-property "position" (make-point 0 0) x))))
)