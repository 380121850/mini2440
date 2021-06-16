#!/usr/bin/zsh

for i in **/moc_*.cpp(.); do
    egrep "^.*strcmp\(.*::.*::className\(\).*$" $i 2>/dev/null 1>/dev/null
    if [ $? = 0 ]; then
    	echo "fixing $i"
        perl -pi -e "s,^(.*strcmp\().*::(.*::className\(\).*)$,\1\2,g" $i
    fi
done

