(defun null (x)
  (if (eq x nil) t nil))

(defun endp (x)
  (if (eq x nil) t nil))

(defun caar (x)
  (car (car x)))

(defun cadr (x)
  (car (cdr x)))

(defun cdar (x)
  (cdr (car x)))

(defun cddr (x)
  (cdr (cdr x)))

(defun rest (x)
  (cdr x))

(defun first (x)
  (car x))

(defun second (x)
  (car (cdr x)))

(defun third (x)
  (car (cdr (cdr x))))

(defun consp (x)
  (not (atom x)))

(defun listp (x)
  (or (eq x nil) (consp x)))

(defmacro push (place item)
  (list 'setq place (list 'cons item place)))

(defmacro pop (place)
  (prog1 (first place)
    (setq place (rest place))))

(defun nreconc (x y)
  (do ((first (cdr x) (if (endp first) first (cdr first)))
       (second x first)
       (third y second))
      ((atom second) third)
    (rplacd second third)))

(defun eql (x y)
  (eq x y))
